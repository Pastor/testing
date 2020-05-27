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
	//FromAddress   = "3kbak.pi@mail.ru"
	FromAddress = "2kbak.pi@mail.ru"
	PortNumber  = 465

	//AttachmentFilePath = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\Задания\\АПСУ"
	AttachmentFilePath = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\Задания\\РПП"
	//InputFilePath      = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\АПСУ.КР.txt"
	//InputFilePath      = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\АПСУ.ЭК.txt"
	//InputFilePath = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\РПП.КР.txt"
	InputFilePath = "E:\\YandexDisk\\YandexDisk\\Учеба\\2020.2\\РПП.ЭК.txt"
	Delimiter     = "==8E6C038B62D14BD2A014D2943594C07C=="
	//WokName            = "«Автоматное программирование систем управления»"
	WokName = "«Разработка программных приложений»"
	//WokShortName       = "АПСУ"
	WokShortName = "РПП"
	Subject      = "О сдаче задолженности по дисциплине " + WokName
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

func sendNotify(username, group, to string, toCopy string, filename string) {
	m := gomail.NewMessage()
	m.SetHeader("From", FromAddress)
	m.SetHeader("To", to, toCopy)
	m.SetHeader("Cc", m.FormatAddress("viruszold@gmail.com", "Хлебников Андрей Александрович"))
	m.SetHeader("Subject", Subject)
	m.SetBody("text/html", GetMessageForNotify(username, group, to))

	d := gomail.NewDialer(ServerAddress, PortNumber, FromAddress, os.Getenv("EMAIL_PASSWORD"))
	if err := d.DialAndSend(m); err != nil {
		log.Panic(err)
	}
	write(fmt.Sprintf("Ф.И.О.: %s, Группа: %s, Задание: %s", username, group, filename))
}

func sendExercise(username, group, to string, toCopy string, filename string) {
	m := gomail.NewMessage()
	m.SetHeader("From", FromAddress)
	m.SetHeader("To", to, toCopy)
	m.SetHeader("Cc", m.FormatAddress("viruszold@gmail.com", "Хлебников Андрей Александрович"))
	m.SetHeader("Subject", Subject)
	m.SetBody("text/html", GetMessageForExercise(username, group, to))
	m.Attach(path.Join(AttachmentFilePath, filename), gomail.Rename("Задание.docx"))

	d := gomail.NewDialer(ServerAddress, PortNumber, FromAddress, os.Getenv("EMAIL_PASSWORD"))
	if err := d.DialAndSend(m); err != nil {
		log.Panic(err)
	}
	write(fmt.Sprintf("Ф.И.О.: %s, Группа: %s, Задание: %s", username, group, filename))
}

func sendCourse(username, group, to string, toCopy string) {
	m := gomail.NewMessage()
	m.SetHeader("From", FromAddress)
	m.SetHeader("To", to, toCopy)
	m.SetHeader("Cc", m.FormatAddress("viruszold@gmail.com", "Хлебников Андрей Александрович"))
	m.SetHeader("Subject", Subject)
	m.SetBody("text/html", GetMessageForCourse(username, group, to))

	d := gomail.NewDialer(ServerAddress, PortNumber, FromAddress, os.Getenv("EMAIL_PASSWORD"))
	if err := d.DialAndSend(m); err != nil {
		log.Panic(err)
	}
	write(fmt.Sprintf("Ф.И.О.: %s, Группа: %s, Оповещение о сдаче курсовой работы", username, group))
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
	sampleMsg += fmt.Sprintf("\r\n%s", GetMessageForExercise(username, group, to))

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

func GetMessageForExercise(username string, group string, to string) string {
	return "<html><body>" +
		"<p>Добрый день " + username + ".</p>" +
		"<p>&nbsp;</p>" +
		"<p>Вы получили это письмо, так как имеете задолженность по дисциплине " + WokName + ". Если вы уже получили оценку по дисциплине, проигнорируйте это письмо.</p>" +
		"<p>К письму прилагается задание, выполнение которого требуется для сдачи дисциплины. На выполнение задания отводится 6 часов.</p>" +
		"<p>Выполните прилагаемое задание в рукописном виде(на листке должно быть указано Ф.И.О.(" + username + "), группа(" + group + "), текст задания) и пришлите фотографию, практическую часть задания следует оформить в виде кода на языке C - одним файлом.</p>" +
		"<p>Архив с файлами(фотографии, файл с исходным кодом) по результатам выполнения задания вышлите со своего адреса почты @edu.mirea.ru(" + to + ", в копию письма добавлена ваша личная почта - <strong><span style=\"color: #ff0000;\">будьте внимательны с личной почты задания приниматься не будут</span></strong>) " +
		"ответным письмом на адрес отправителя этого письма  <strong><span style=\"color: #ff0000;\">ДО 16:00 27 МАЯ 2020 года</span></strong>. В теме письма укажите «" + WokShortName + " Экзамен», также, в теле письма укажите дополнительно Ф.И.О.(" + username + "), группу(" + group + ").</p>" +
		"<p><strong><span style=\"color: #ff0000;\">Решенные задания, полученные позже указанного времени и даты или не соответствующие требованиям указанным в этом письме, проверяться не будут. Если вы не сдавали курсовую работу по " + WokName + " экзамен не будет засчитан, даже при удачной сдаче.</span></strong></p>" +
		"<p>Результат проверки будет доведен до вас до начала сессии.\n\n<p>С уважением,</p><p>&nbsp; &nbsp; &nbsp;администрация кафедры промышленной информатики (ПИ)</p>" +
		"</body></html>\r\n"
}

func GetMessageForCourse(username string, group string, to string) string {
	return "<html><body>" +
		"<p>Добрый день " + username + ".</p>" +
		"<p>&nbsp;</p>" +
		"<p>Вы получили это письмо, так как имеете задолженность по дисциплине " + WokName + " курсовая работа.</p>" +
		"<p>Архив с курсовой работой вышлите со своего адреса почты @edu.mirea.ru(" + to + ", в копию письма добавлена ваша личная почта - <strong><span style=\"color: #ff0000;\">будьте внимательны с личной почты задания приниматься не будут</span></strong>) " +
		"ответным письмом на адрес отправителя этого письма  <strong><span style=\"color: #ff0000;\">ДО 15 МАЯ 2020 года</span></strong>, т.е. 14 МАЯ 2020 года работы не будут приниматься к рассмотрению. В теме письма укажите «" + WokShortName + " Курсовая», также, в теле письма укажите дополнительно Ф.И.О.(" + username + "), группу(" + group + ").</p>" +
		"<p>Если вы не получали задания ранее, отошлите письмо (с требованиями указанными выше) и в теле письма напишите, что задание вам не выдавалось. </p>" +
		"<p><strong><span style=\"color: #ff0000;\">Курсовые работы, полученные позже указанной даты, проверяться не будут.</span></strong> В деканат, результаты будут переданы 15 МАЯ 2020 года.</p>" +
		"<p>Результат проверки будет доведен до вас до начала сессии.\n\n<p>С уважением,</p><p>&nbsp; &nbsp; &nbsp;администрация кафедры промышленной информатики (ПИ)</p>" +
		"</body></html>\r\n"
}

func GetMessageForNotify(username string, group string, to string) string {
	return "<html><body>" +
		"<p>Добрый день " + username + ".</p>" +
		"<p>&nbsp;</p>" +
		"<p>Вы получили это письмо, так как имеете задолженность по дисциплине " + WokName + " экзамен.</p>" +
		"<p><strong><span style=\"color: #ff0000;\">27 мая 2020 года будет происходить переэкзаменовка по дисциплине.</span></strong> </p>" +
		"<p>Утром, 27 числа вам будет выслано задание на почту \"" + to + "\". Время на выполнение работы ~ 4 - 5 часов. Просьба подготовится к сдаче и присутствовать за компьютером.</p>" +
		"<p>В случае невозможности участия в переэкзаменовке, просьба оповестить заранее. О времени пересдач в другие дни, пока не известно.</p>" +
		"<p>Если вы уже сдавали экзамен и получили оценку просьба проигнорировать письмо.</p>" +
		"<p><strong><span style=\"color: #ff0000;\">Любые вопросы по пересдаче присылайте со своей электронной почты(" + to + "), в теме письма указывайте \"РПП Экзамен\". " +
		"На письма не соответствующие данным требованиям ответа вы не получите.</span></strong></p>" +
		"<p>С уважением,</p><p>&nbsp; &nbsp; &nbsp;администрация кафедры промышленной информатики (ПИ)</p>" +
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
		n := (i % 29) + 1
		//sendNotify(username, group, email, emailCopy, fmt.Sprintf("%d.docx", n))
		sendExercise(username, group, email, emailCopy, fmt.Sprintf("%d.docx", n))
		//sendCourse(username, group, email, emailCopy)
		log.Printf("'%s', '%s', '%s'('%s'), '%d.docx'", username, group, email, emailCopy, n)
		i++
		time.Sleep(5 * time.Second)
	}
	file.Close()
}
