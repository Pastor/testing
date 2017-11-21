package main

func main() {
	var server = start(LoadConfigurationFromFile("config/v1.yml").Context)
	server.ListenAndServe()
}
