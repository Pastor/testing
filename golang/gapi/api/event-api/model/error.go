package model

type (
	//Error
	Error struct {
		Code    string `json:"code"`
		Message string `json:"message"`
	}
)
