package main

type Exception struct {
	Code    string `json:"error_code"`
	Message string `json:"error_message"`
}

type ErrorType struct {
	Exception Exception
	HttpCode  int
}

var (
	IllegalContentTypeError = ErrorType{
		Exception: Exception{Code: "BPE-002001", Message: "Неверный Content-Type HTTP-запроса"}, HttpCode: 400}
	CantReadContentError = ErrorType{
		Exception: Exception{Code: "BPE-002003", Message: "Не удалось прочитать биометрический образец"}, HttpCode: 400}
	CantExtractBiometricsError = ErrorType{
		Exception: Exception{Code: "BPE-002004", Message: "Не удалось извлечь биометрический шаблон"}, HttpCode: 400}
	InternalServiceError = ErrorType{
		Exception: Exception{Code: "BPE-001001", Message: "Внутренняя ошибка биопроцессора"}, HttpCode: 500}
)