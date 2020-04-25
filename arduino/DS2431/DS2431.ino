//BYTE, BIN (двоичный), OCT (восьмиричный), DEC (десятеричный), HEX (шестнадцатеричный)

#include <OneWire.h>

OneWire  ds(6);    // 1-wire on pin 2
byte     addr[8];  // Contains the eeprom unique ID //Этой колонке указан EEPROM уникальный идентификатор
byte     led = 13;
//-------------------------------------------------------------------------

void setup(void)  // запускается только один раз рп истарте

{
  Serial.begin(9600); // ативацию сериал соединения для в/вывода на экран
  pinMode(led, OUTPUT);
}
// ----------------------------------------------------------------------

void loop(void) // функцию повторяющаяся постоянно

{
  byte i;  // инициализация переменно как байты
  byte dat[13];  // инициалтзация массива как байт

  SearchAddress(addr); // функция поиска адреса

  dat[0] = 0x01;
  dat[1] = 0x02;
  dat[2] = 0x03;
  dat[3] = 0x04;
  dat[4] = 0x05;
  dat[5] = 0x06;
  dat[6] = 0x07;
  dat[7] = 0x08;

  //WriteRow(0,dat); // функция записи
  ReadAllMem();    //print all mem content//печатать все сообщения MEMогу// функция чтения

  digitalWrite(led, HIGH);   // set the LED on
  delay(1000);                  // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(1000);                  // wait for a second
}
//-----------------------------------------------------------------------------------------

void SearchAddress(byte* address) //Search for address and confirm it//функция Поиск адреса

{
  int i; // инициализациия переменной как числовое (от -2^15 до -2^14)
  if ( !ds.search(address)) //операттор "если" неДС то
  {
    Serial.print("No device found.\n"); // отправить в серил "нет устройства"
    ds.reset_search(); // перезапуск поиска устройства
    delay(250); //пауза в 0.25 сек
    return; // сброс функции
  }

  Serial.print("ADDR= ");
  for ( i = 0; i < 8; i++)
  {
    Serial.print(address[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( address, 7) != address[7])
  {
    Serial.print("CRC is not valid, address is corrupted\n");
    return;
  }

  if ( address[0] != 0x2D)
  {
    Serial.print("Device is not a 1-wire Eeprom.\n");
    return;
  }
  Serial.println();
}
//-------------------------------------------------------------------------------------------

void WriteReadScratchPad(byte TA1, byte TA2, byte* data)//чтение-запись в сверхоперативную память

{
  int i;
  ds.reset();
  ds.select(addr);
  ds.write(0x0F, 1); // Write ScratchPad//Написать свПамять
  ds.write(TA1, 1);
  ds.write(TA2, 1);
  for ( i = 0; i < 8; i++)
    ds.write(data[i], 1);

  ds.reset();
  ds.select(addr);
  ds.write(0xAA);         // Read Scratchpad// чтение свпамяти

  for ( i = 0; i < 13; i++)
    data[i] = ds.read();
}
//---------------------------------------------------------------------------------------------------------------

void CopyScratchPad(byte* data)        //функция копирование из сверхоперативной памяти в основную память

{
  ds.reset(); //перезагрузка соединения
  ds.select(addr); // выбор адреса
  ds.write(0x55, 1); // Copy ScratchPad//копировать из сверхопер памяти в основную
  ds.write(data[0], 1);
  ds.write(data[1], 1); // Send TA1 TA2 and ES for copy authorization//
  ds.write(data[2], 1);
  delay(25); // Waiting for copy completion
  //Serial.print("Copy done!\n");
}

//------------------------------------------------------------------------------------------------------------------

void ReadAllMem() //функция чтения памяти

{
  int i;
  ds.reset();
  ds.select(addr);
  ds.write(0xF0, 1); // Read Memory
  ds.write(0x00, 1); //Read Offset 0000h
  ds.write(0x00, 1);

  for ( i = 0; i < 128; i++) //whole mem is 144 //вся память состоит из 144 байт
  {
    Serial.print(ds.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
}
//------------------------------------------------------------------------------------------------


void WriteRow(byte row, byte* buffer) {
  int i;
  if (row < 0 || row > 15) //There are 16 row of 8 bytes in the main memory//Существуют 16 строке 8 байт в основной памяти
    return;                //The remaining are for the 64 bits register page//Остальные предназначены для 64 бит Страницы регистрации

  WriteReadScratchPad(row * 8, 0x00, buffer);

  /*  Print result of the ReadScratchPad//Распечатать результат ReadScratchPad
    for ( i = 0; i < 13; i++)
    {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
    }
  */
  CopyScratchPad(buffer);

}
