package main

//go get -u github.com/swaggo/swag/cmd/swag
import (
	_ "./docs"
	"encoding/json"
	"errors"
	"fmt"
	"github.com/gin-contrib/logger"
	"github.com/gin-gonic/contrib/static"
	"github.com/gin-gonic/gin"
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
	ginSwagger "github.com/swaggo/gin-swagger"
	"github.com/swaggo/gin-swagger/swaggerFiles"
	"github.com/swaggo/swag/example/celler/httputil"
	"net/http"
	"os"
	"strings"
	"time"
)

type (
	LocalDate time.Time
	//Event
	Event struct {
		//ID        uint      `json:"id"`
		Title     string    `json:"title" binding:"required"`
		CreatedAt LocalDate `json:"created_at" swaggertype:"primitive,string"`
	}
)

func (local *LocalDate) UnmarshalJSON(b []byte) error {
	s := strings.Trim(string(b), "\"")
	t, err := time.Parse("2019-01-10", s)
	if err != nil {
		return err
	}
	*local = LocalDate(t)
	return nil
}

func (local LocalDate) MarshalJSON() ([]byte, error) {
	return json.Marshal(time.Time(local))
}

func (local LocalDate) Format(s string) string {
	t := time.Time(local)
	return t.Format(s)
}

// GetEvent godoc
// @Summary Get event by ID
// @Description get event by ID
// @ID get-event-by-int
// @Accept  json
// @Produce  json
// @Param id path int true "Event ID"
// @Success 200 {object} main.Event
// @Failure 400 {object} model.Error
// @Failure 404 {object} model.Error
// @Failure 500 {object} model.Error
// @Router /events/{id} [get]
func GetEvent(c *gin.Context) {
	id := c.Param("id")
	c.JSON(http.StatusOK, gin.H{
		"message": fmt.Sprintf("GET event by %s: ", id),
	})
}

// GetEvents godoc
// @Summary Get all events
// @Description get all events
// @ID get-events
// @Accept  json
// @Produce  json
// @Success 200 {array} main.Event
// @Failure 400 {object} model.Error
// @Failure 404 {object} model.Error
// @Failure 500 {object} model.Error
// @Router /events/ [get]
func GetEvents(c *gin.Context) {
	events := make([]Event, 0)
	events = append(events, Event{Title: "Default", CreatedAt: LocalDate(time.Now())})
	c.JSON(http.StatusOK, events)
}

// CreateEvent godoc
// @Summary Create event
// @Description create event
// @ID create-event
// @Accept  json
// @Produce  json
// @Consume  json
// @Param event body main.Event true "Create event"
// @Success 201 {object} main.Event
// @Failure 400 {object} model.Error
// @Failure 404 {object} model.Error
// @Failure 500 {object} model.Error
// @Router /events/ [post]
func CreateEvent(c *gin.Context) {
	var event Event
	if err := c.ShouldBindJSON(&event); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"message": "Bad event structure"})
		return
	}
	c.JSON(http.StatusCreated, gin.H{
		"message": fmt.Sprintf("Event %v created", event),
	})
}

// ModifyEvent godoc
// @Summary Modify event
// @Description modify event
// @ID modify-event
// @Accept  json
// @Produce  json
// @Consume  json
// @Param id path int true "Event ID"
// @Param event body main.Event true "Modify event"
// @Success 200 {object} main.Event
// @Failure 400 {object} model.Error
// @Failure 404 {object} model.Error
// @Failure 500 {object} model.Error
// @Router /events/{id} [put]
func ModifyEvent(c *gin.Context) {
	id := c.Param("id")
	c.JSON(http.StatusOK, gin.H{
		"message": fmt.Sprintf("modify event by %s: ", id),
	})
}

// @title Swagger Example API
// @version 1.0
// @description This is a sample server celler server.
// @termsOfService http://swagger.io/terms/

// @contact.name API Support
// @contact.url http://www.swagger.io/support
// @contact.email support@swagger.io

// @license.name Apache 2.0
// @license.url http://www.apache.org/licenses/LICENSE-2.0.html

// @host localhost:8080
// @BasePath /

// @securityDefinitions.basic BasicAuth

// @securityDefinitions.apikey ApiKeyAuth
// @in header
// @name Authorization

// @securitydefinitions.oauth2.application OAuth2Application
// @tokenUrl https://example.com/oauth/token
// @scope.write Grants write access
// @scope.admin Grants read and write access to administrative information

// @securitydefinitions.oauth2.implicit OAuth2Implicit
// @authorizationurl https://example.com/oauth/authorize
// @scope.write Grants write access
// @scope.admin Grants read and write access to administrative information

// @securitydefinitions.oauth2.password OAuth2Password
// @tokenUrl https://example.com/oauth/token
// @scope.read Grants read access
// @scope.write Grants write access
// @scope.admin Grants read and write access to administrative information

// @securitydefinitions.oauth2.accessCode OAuth2AccessCode
// @tokenUrl https://example.com/oauth/token
// @authorizationurl https://example.com/oauth/authorize
// @scope.admin Grants read and write access to administrative information

func main() {

	zerolog.SetGlobalLevel(zerolog.InfoLevel)
	if gin.IsDebugging() {
		zerolog.SetGlobalLevel(zerolog.DebugLevel)
	}

	log.Logger = log.Output(
		zerolog.ConsoleWriter{
			Out:     os.Stderr,
			NoColor: false,
		},
	)

	router := gin.Default()

	router.Use(logger.SetLogger())
	router.Use(static.Serve("/", static.LocalFile("../client", true)))

	system := router.Group("/system")
	{
		system.Use(auth())
		system.GET("/events")
	}

	events := router.Group("/events")
	{

		events.GET("/", GetEvents)
		events.GET("/:id", GetEvent)
		events.PUT("/:id", ModifyEvent)
		events.POST("/", CreateEvent)

	}
	router.GET("/swagger/*any", ginSwagger.WrapHandler(swaggerFiles.Handler))
	router.Run(":8080")
}

func auth() gin.HandlerFunc {
	return func(c *gin.Context) {
		if len(c.GetHeader("Authorization")) == 0 {
			httputil.NewError(c, http.StatusUnauthorized, errors.New("Authorization is required Header"))
			c.Abort()
		}
		c.Next()
	}
}
