
 //**************************************************************************//
 //  ������������ ������� ��� ������ � ���������� ��������� ����������� ���� //
 //**************************************************************************//
 // �����������:        �������� �.�.                                        //
 //**************************************************************************//

#include "windows.h"
#include "sit_sparce_interface.h"

typedef p_sparce_solver_interface(sparce_solver_call_type *t_get_interface_func)(char* lae_solver_type_name);

  //���c� � ����������� ����������� �������-�������� ����
  //���������� ������������� ���������� �������� ������� lae_solver_id, ���� 0 - ��� ����� �������
sp_solve_id lae_solver_register(solver_struct* solver_data, char* lae_solver_name, char* library_name, p_sparce_solver_interface* sp_solver_interface)
{
	sp_solve_id  result;
    HINSTANCE aLibHandle;
	t_get_interface_func  get_interface_proc;

	/* �������� ������ ���������� � ����� ���� */
	result = solver_data->FindGlobalObject(solver_data->LayerContext, lae_solver_name);
	if (result == 0) {

		//������ ������ ������
   	   aLibHandle = solver_data->DoLoadNeedPlugin(library_name);
	   
	   if (aLibHandle != 0) {
		  //���� ���������, �� ��������� � ���� ���������
	      get_interface_proc = (t_get_interface_func)GetProcAddress(aLibHandle, "get_object_interface");

	      //���� ��������� ����, �� �������� �� ���� ������ � ������ ��������� �������
	      if (get_interface_proc != 0) {	
			 *sp_solver_interface = get_interface_proc(0);
			 result = (*sp_solver_interface)->lae_solver_create();
		     solver_data->RegisterGlobalObject(solver_data->LayerContext, lae_solver_name, result, (*sp_solver_interface)->lae_solver_free);
	      };

       };

	}else{

	   //������� ������ �� ��������� �������� � ������������� ���������� ������ �������� (���� ������� ���� ...)	
		*sp_solver_interface = lae_solver_getinterface(result); 

	};

	return result;
};

 //����� ������������� ������ �������� ������� ����������� ��� - � ������ ������ ��� ��������� ������ !
void lae_solver_initialization(solver_struct* solver_data)
{


 return;
};

 //�������� ����������� ���� �� ����� - � ������ ������ ��� ��������� ������ !
void lae_solver_finalization(solver_struct* solver_data)
{


 return;
};


