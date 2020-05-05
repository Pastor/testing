package com.example.smspoll

import android.os.Bundle
import android.telephony.SmsManager
import android.view.View
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    fun sendSMS(view: View) {
        sms("+79260110405", "Тестовое сообщение")
    }

    fun sms(phoneNo: String, msg: String) {
        try {
            val smsManager: SmsManager = SmsManager.getDefault()
            smsManager.sendTextMessage(phoneNo, null, msg, null, null)
            Toast.makeText(applicationContext, "Message Sent",
                    Toast.LENGTH_LONG).show()
        } catch (ex: Exception) {
            Toast.makeText(applicationContext, ex.message.toString(),
                    Toast.LENGTH_LONG).show()
            ex.printStackTrace()
        }
    }
}
