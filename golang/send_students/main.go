package main

import (
	"bufio"
	"crypto/tls"
	"encoding/base64"
	"fmt"
	"gopkg.in/gomail.v2"
	"io/ioutil"
	"log"
	"net/smtp"
	"os"
	"path"
	"strings"
	"time"
)

const (
	ServerAddress = "smtp.mail.ru"
	FromAddress   = "3kbak.pi@mail.ru"
	PortNumber    = 465

	AttachmentFilePath = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\Задания\\АПСУ"
	InputFilePath      = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\АПСУ.ЭК.txt"
	Delimiter          = "==8E6C038B62D14BD2A014D2943594C07C=="
	Subject            = "О сдаче задолженности по дисциплине «Автоматное программирование систем управления»"
)

func write(message string) {
	f, err := os.OpenFile("sent.txt", os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0644)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer f.Close()
	_, _ = fmt.Fprintln(f, fmt.Sprintf("[%v] %v", time.Now(), message))
}

func send2(username, group, to string, toCopy string, filename string) {
	m := gomail.NewMessage()
	m.SetHeader("From", FromAddress)
	m.SetHeader("To", to, toCopy)
	m.SetHeader("Cc", m.FormatAddress("viruszold@gmail.com", "Хлебников Андрей Александрович"))
	m.SetHeader("Subject", Subject)
	m.SetBody("text/html", GetMessage(username, group, to))
	m.Attach(path.Join(AttachmentFilePath, filename), gomail.Rename("Задание.docx"))

	d := gomail.NewDialer(ServerAddress, PortNumber, FromAddress, os.Getenv("EMAIL_PASSWORD"))
	if err := d.DialAndSend(m); err != nil {
		log.Panic(err)
	}
	write(fmt.Sprintf("Ф.И.О.: %s, Группа: %s, Задание: %s", username, group, filename))
}

func send(username, group, to string, toCopy string, filename string) {
	tlsConfig := tls.Config{
		ServerName:         ServerAddress,
		InsecureSkipVerify: true,
	}
	conn, connErr := tls.Dial("tcp", fmt.Sprintf("%s:%d", ServerAddress, PortNumber), &tlsConfig)
	if connErr != nil {
		log.Panic(connErr)
	}
	defer conn.Close()
	client, clientErr := smtp.NewClient(conn, ServerAddress)
	if clientErr != nil {
		log.Panic(clientErr)
	}
	defer client.Close()
	auth := smtp.PlainAuth("", FromAddress, os.Getenv("EMAIL_PASSWORD"), ServerAddress)

	if err := client.Auth(auth); err != nil {
		log.Panic(err)
	}

	if err := client.Mail(FromAddress); err != nil {
		log.Panic(err)
	}
	if err := client.Rcpt(to); err != nil {
		log.Panic(err)
	}

	writer, writerErr := client.Data()
	if writerErr != nil {
		log.Panic(writerErr)
	}

	//basic email headers
	sampleMsg := fmt.Sprintf("From: %s\r\n", FromAddress)
	sampleMsg += fmt.Sprintf("To: %s\r\n", to)
	sampleMsg += fmt.Sprintf("Cc: %s\r\n", "viruszold@mail.ru;"+toCopy)
	sampleMsg += fmt.Sprintf("Subject: %s\r\n", Subject)

	sampleMsg += "MIME-Version: 1.0\r\n"
	sampleMsg += fmt.Sprintf("Content-Type: multipart/mixed; boundary=\"%s\"\r\n", Delimiter)

	sampleMsg += fmt.Sprintf("\r\n--%s\r\n", Delimiter)
	sampleMsg += "Content-Type: text/html; charset=\"utf-8\"\r\n"
	sampleMsg += "Content-Transfer-Encoding: 7bit\r\n"
	sampleMsg += fmt.Sprintf("\r\n%s", GetMessage(username, group, to))

	sampleMsg += fmt.Sprintf("\r\n--%s\r\n", Delimiter)
	sampleMsg += "Content-Type: text/plain; charset=\"utf-8\"\r\n"
	sampleMsg += "Content-Transfer-Encoding: base64\r\n"
	sampleMsg += "Content-Disposition: attachment;filename=\"Задание.docx\"\r\n"
	//read file
	rawFile, fileErr := ioutil.ReadFile(path.Join(AttachmentFilePath, filename))
	if fileErr != nil {
		log.Panic(fileErr)
	}
	sampleMsg += "\r\n" + base64.StdEncoding.EncodeToString(rawFile)
	if _, err := writer.Write([]byte(sampleMsg)); err != nil {
		log.Panic(err)
	}
	if closeErr := writer.Close(); closeErr != nil {
		log.Panic(closeErr)
	}
	client.Quit()
	write(fmt.Sprintf("Ф.И.О.: %s, Группа: %s, Задание: %s", username, group, filename))
}

func GetMessage(username string, group string, to string) string {
	return "<html><body>" +
		"Добрый день " + username + ".\n\n" +
		"<p>Вы получили это письмо, так как имеете задолженность по дисциплине «Автоматное программирование систем управления».\n" +
		"<p>К письму прилагается задание, выполнение которого требуется для сдачи дисциплины.\n" +
		"<p>Выполните прилагаемое задание в рукописном виде(на листке должно быть указано Ф.И.О.(" + username + "), группа(" + group + "), задания) и пришлите фотографию, практическую часть задания следует оформить в виде кода на языке C - одним файлом\n" +
		"<p>Архив с файлами по результатам выполнения задания вышлите со своего адреса почты @edu.mirea.ru(" + to + ", в копию письма добавлена ваша личная почта - будьте внимательны с личной почты задания приниматься не будут) " +
		"ответным письмом на адрес отправителя этого письма  ДО 7 МАЯ 2020 года. В теме письма укажите «АПСУ Экзамен», также, в теле письма укажите дополнительно Ф.И.О.(" + username + "), группу(" + group + ").\n" +
		"<p>Решенные задания, полученные позже указанной даты, проверяться не будут.\n" +
		"<p>Результат проверки задания будет доведен до вас до начала сессии.\n\n<p><p>С уважением,\n<br>администрация кафедры промышленной информатики (ПИ)\n" +
		"</body></html>\r\n"
}

func main() {
	file, err := os.Open(InputFilePath)

	if err != nil {
		log.Fatalf("failed opening file: %s", err)
	}

	scanner := bufio.NewScanner(file)
	scanner.Split(bufio.ScanLines)
	var i = 1
	for scanner.Scan() {
		line := scanner.Text()
		if len(line) == 0 {
			continue
		}
		line = strings.ReplaceAll(line, "\t", " ")
		parts := strings.Split(line, ";")
		username := strings.TrimSpace(parts[0])
		email := strings.TrimSpace(parts[1])
		emailCopy := strings.TrimSpace(parts[2])
		group := strings.TrimSpace(parts[4])
		send2(username, group, email, emailCopy, fmt.Sprintf("%d.docx", i))
		log.Printf("'%s', '%s', '%s', '%d.docx'", username, group, email, i)
		i++
		time.Sleep(5 * time.Second)
	}
	file.Close()
}
