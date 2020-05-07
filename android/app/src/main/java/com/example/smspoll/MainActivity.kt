package com.example.smspoll

import android.content.Context
import android.os.*
import android.telephony.SmsManager
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import org.json.JSONObject
import java.util.concurrent.TimeUnit


class MainActivity : AppCompatActivity() {
    private val client = OkHttpClient.Builder()
        .callTimeout(120, TimeUnit.SECONDS)
        .readTimeout(120, TimeUnit.SECONDS)
        .writeTimeout(120, TimeUnit.SECONDS)
        .build()
    private val longPoll = LongPoll(client, object : EventNotify {
        override fun onEvent(eventsText: String) {
            val receive = JSONObject(eventsText)
            if (receive.has("events")) {
                val events = receive.getJSONArray("events")
                for (i in 0 until events.length()) {
                    val event = events.getJSONObject(i)
                    val smsData = event.getJSONObject("data")
                    sms(smsData.getString("to"), smsData.getString("text"))
                }
            }
        }
    })

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val preferences = getPreferences(Context.MODE_PRIVATE)
        val input = findViewById<TextView>(R.id.serverAddress)
        input.text =
            preferences.getString(URL_KEY, "http://192.168.2.177:8091/api/events")
    }

    fun connect(view: View) {
        val button = findViewById<Button>(R.id.btnConnect)
        button.isEnabled = longPoll.isCancelled
        if (longPoll.status == AsyncTask.Status.RUNNING)
            return
        val input = findViewById<TextView>(R.id.serverAddress)
        val url = input?.text?.toString() ?: "http://192.168.2.177:8091/api/events"
        val preferences = getPreferences(Context.MODE_PRIVATE)
        val editor = preferences.edit()
        editor.putString(URL_KEY, url)
        editor.apply()
        longPoll.execute("$url?timeout=45&category=sms")
        button.isEnabled = false
    }

    fun sms(phoneNo: String, msg: String) {
        try {
            val smsManager: SmsManager = SmsManager.getDefault()
            smsManager.sendTextMessage(phoneNo, null, msg, null, null)
        } catch (ex: Exception) {
            object : Handler(Looper.getMainLooper()) {
                override fun handleMessage(msg: Message?) {
                    Toast.makeText(
                        applicationContext, "Ошибка отправки: " + ex.localizedMessage,
                        Toast.LENGTH_LONG
                    ).show()
                }
            }.sendEmptyMessage(0)
        }
    }

    class LongPoll(private val client: OkHttpClient, private val eventNotify: EventNotify) :
        AsyncTask<String, Response, Unit>() {
        override fun doInBackground(vararg params: String?) {
            while (!isCancelled) {
                val url = params[0]
                val request =
                    Request.Builder().url(url!!).post(ByteArray(0).toRequestBody()).build()
                val execute = client.newCall(request).execute()
                onProgressUpdate(execute)
            }
        }

        override fun onProgressUpdate(vararg values: Response?) {
            super.onProgressUpdate(*values)
            values.apply {
                if (this.isEmpty())
                    return@apply
                this[0]?.body?.apply {
                    val content = this.string()
                    Log.d("SMS", content)
                    if (content.isNotEmpty())
                        eventNotify.onEvent(content)
                    this.close()
                }
            }
        }
    }

    interface EventNotify {
        fun onEvent(eventsText: String)
    }

    companion object {
        private const val URL_KEY = "SMS_Poll_Url"
    }
}
