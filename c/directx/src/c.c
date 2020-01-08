#include <Wincodec.h>             // we use WIC for saving images
  #include <d3d9.h>                 // DirectX 9 header
  #pragma comment(lib, "d3d9.lib")  // link to DirectX 9 library

  //https://docs.microsoft.com/ru-ru/windows/win32/direct3ddxgi/desktop-dup-api?redirectedfrom=MSDN

  #define WIDEN2(x) L ## x
  #define WIDEN(x) WIDEN2(x)
  #define __WFILE__ WIDEN(__FILE__)
  #define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);goto cleanup;}}
  #define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

  HRESULT Direct3D9TakeScreenshots(UINT adapter, UINT count)
  {
    HRESULT hr = S_OK;
    IDirect3D9 *d3d = nullptr;
    IDirect3DDevice9 *device = nullptr;
    IDirect3DSurface9 *surface = nullptr;
    D3DPRESENT_PARAMETERS parameters = { 0 };
    D3DDISPLAYMODE mode;
    D3DLOCKED_RECT rc;
    UINT pitch;
    SYSTEMTIME st;
    LPBYTE *shots = nullptr;

    // init D3D and get screen size
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    HRCHECK(d3d->GetAdapterDisplayMode(adapter, &mode));

    parameters.Windowed = TRUE;
    parameters.BackBufferCount = 1;
    parameters.BackBufferHeight = mode.Height;
    parameters.BackBufferWidth = mode.Width;
    parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    parameters.hDeviceWindow = NULL;

    // create device & capture surface
    HRCHECK(d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device));
    HRCHECK(device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr));

    // compute the required buffer size
    HRCHECK(surface->LockRect(&rc, NULL, 0));
    pitch = rc.Pitch;
    HRCHECK(surface->UnlockRect());

    // allocate screenshots buffers
    shots = new LPBYTE[count];
    for (UINT i = 0; i < count; i++)
    {
      shots[i] = new BYTE[pitch * mode.Height];
    }

    GetSystemTime(&st); // measure the time we spend doing <count> captures
    wprintf(L"%i:%i:%i.%i\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    for (UINT i = 0; i < count; i++)
    {
      // get the data
      HRCHECK(device->GetFrontBufferData(0, surface));

      // copy it into our buffers
      HRCHECK(surface->LockRect(&rc, NULL, 0));
      CopyMemory(shots[i], rc.pBits, rc.Pitch * mode.Height);
      HRCHECK(surface->UnlockRect());
    }
    GetSystemTime(&st);
    wprintf(L"%i:%i:%i.%i\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    // save all screenshots
    for (UINT i = 0; i < count; i++)
    {
      WCHAR file[100];
      wsprintf(file, L"cap%i.png", i);
      HRCHECK(SavePixelsToFile32bppPBGRA(mode.Width, mode.Height, pitch, shots[i], file, GUID_ContainerFormatPng));
    }

  cleanup:
    if (shots != nullptr)
    {
      for (UINT i = 0; i < count; i++)
      {
        delete shots[i];
      }
      delete[] shots;
    }
    RELEASE(surface);
    RELEASE(device);
    RELEASE(d3d);
    return hr;
  }




   HRESULT SavePixelsToFile32bppPBGRA(UINT width, UINT height, UINT stride, LPBYTE pixels, LPWSTR filePath, const GUID &format)
  {
    if (!filePath || !pixels)
      return E_INVALIDARG;

    HRESULT hr = S_OK;
    IWICImagingFactory *factory = nullptr;
    IWICBitmapEncoder *encoder = nullptr;
    IWICBitmapFrameEncode *frame = nullptr;
    IWICStream *stream = nullptr;
    GUID pf = GUID_WICPixelFormat32bppPBGRA;
    BOOL coInit = CoInitialize(nullptr);

    HRCHECK(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));
    HRCHECK(factory->CreateStream(&stream));
    HRCHECK(stream->InitializeFromFilename(filePath, GENERIC_WRITE));
    HRCHECK(factory->CreateEncoder(format, nullptr, &encoder));
    HRCHECK(encoder->Initialize(stream, WICBitmapEncoderNoCache));
    HRCHECK(encoder->CreateNewFrame(&frame, nullptr)); // we don't use options here
    HRCHECK(frame->Initialize(nullptr)); // we dont' use any options here
    HRCHECK(frame->SetSize(width, height));
    HRCHECK(frame->SetPixelFormat(&pf));
    HRCHECK(frame->WritePixels(height, stride, stride * height, pixels));
    HRCHECK(frame->Commit());
    HRCHECK(encoder->Commit());

  cleanup:
    RELEASE(stream);
    RELEASE(frame);
    RELEASE(encoder);
    RELEASE(factory);
    if (coInit) CoUninitialize();
    return hr;
  }



  int _tmain(int argc, _TCHAR* argv[])
  {
    HRESULT hr = Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT, 10);
    return 0;
  }