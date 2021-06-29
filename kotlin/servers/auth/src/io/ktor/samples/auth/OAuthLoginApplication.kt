package io.ktor.samples.auth

import io.ktor.application.*
import io.ktor.auth.*
import io.ktor.client.*
import io.ktor.client.engine.apache.*
import io.ktor.config.*
import io.ktor.features.*
import io.ktor.html.*
import io.ktor.locations.*
import io.ktor.request.*
import io.ktor.routing.*
import kotlinx.html.*
import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable
import kotlinx.serialization.builtins.ListSerializer
import kotlinx.serialization.json.Json

@Location("/")
class Index()

@Location("/login/{type?}")
class Login(val type: String = "")

@Serializable
data class Provider(
    @SerialName("name") val name: String,
    @SerialName("request_token_url") val requestTokenUrl: String = "",
    @SerialName("authorize_url") val authorizeUrl: String,
    @SerialName("access_token_url") val accessTokenUrl: String,
    @SerialName("client_id") val clientId: String,
    @SerialName("client_secret") val clientSecret: String,
    @SerialName("userinfo_url") val userInfo: String = "",
)

data class ProviderInfo(val oauth: OAuthServerSettings, val provider: Provider)

object Providers {
    private val repository = HashMap<String, ProviderInfo>()

    fun load(config: ApplicationConfig) {
        Providers.javaClass.classLoader.getResourceAsStream("providers.json").use { stream ->
            val providers =
                Json.decodeFromString(ListSerializer(Provider.serializer()), stream!!.readAllBytes().decodeToString())
            providers.forEach {
                repository[it.name] = ProviderInfo(
                    OAuthServerSettings.OAuth2ServerSettings(
                        name = it.name,
                        authorizeUrl = it.authorizeUrl,
                        accessTokenUrl = it.accessTokenUrl,
                        clientId = it.clientId,
                        clientSecret = it.clientSecret
                    ), it
                )
            }
        }
    }

    operator fun get(name: String) = repository[name]

    operator fun iterator() = repository.iterator()
}

fun Application.main() {
    Providers.load(environment.config.config("providers"))
    loginApplicationWithDeps(
        oauthHttpClient = HttpClient(Apache).apply {
            environment.monitor.subscribe(ApplicationStopping) {
                close()
            }
        }
    )
}

fun Application.loginApplicationWithDeps(oauthHttpClient: HttpClient) {
    val authOauthForLogin = "authOauthForLogin"

    install(DefaultHeaders)
    install(CallLogging)
    install(Locations)
    install(Authentication) {
        oauth(authOauthForLogin) {
            client = oauthHttpClient
            providerLookup = {
                Providers[application.locations.resolve<Login>(Login::class, this).type]?.oauth
            }
            urlProvider = { p -> redirectUrl(Login(p.name), false) }
        }
    }


    install(Routing) {
        get<Index> {
            call.respondHtml {
                head {
                    title { +"index page" }
                }
                body {
                    h1 {
                        +"Try to login"
                    }
                    p {
                        a(href = locations.href(Login())) {
                            +"Login"
                        }
                    }
                }
            }
        }

        authenticate(authOauthForLogin) {
            location<Login>() {
                param("error") {
                    handle {
                        call.loginFailedPage(call.parameters.getAll("error").orEmpty())
                    }
                }

                handle {
                    val principal = call.authentication.principal<OAuthAccessTokenResponse>()
                    if (principal != null) {
                        call.loggedInSuccessResponse(principal)
                    } else {
                        call.loginPage()
                    }
                }
            }
        }
    }
}

private fun <T : Any> ApplicationCall.redirectUrl(t: T, secure: Boolean = true): String {
    val hostPort = request.host() + request.port().let { port -> if (port == 80) "" else ":$port" }
    val protocol = when {
        secure -> "https"
        else -> "http"
    }
    return "$protocol://$hostPort${application.locations.href(t)}"
}

private suspend fun ApplicationCall.loginPage() {
    respondHtml {
        head {
            title { +"Login with" }
        }
        body {
            h1 {
                +"Login with:"
            }

            for (p in Providers) {
                p {
                    a(href = application.locations.href(Login(p.key))) {
                        +p.key
                    }
                }
            }
        }
    }
}

private suspend fun ApplicationCall.loginFailedPage(errors: List<String>) {
    respondHtml {
        head {
            title { +"Login with" }
        }
        body {
            h1 {
                +"Login error"
            }

            for (e in errors) {
                p {
                    +e
                }
            }
        }
    }
}

private suspend fun ApplicationCall.loggedInSuccessResponse(callback: OAuthAccessTokenResponse) {
    respondHtml {
        head {
            title { +"Logged in" }
        }
        body {
            h1 {
                +"You are logged in"
            }
            p {
                +"Your token is $callback"
            }
        }
    }
}
