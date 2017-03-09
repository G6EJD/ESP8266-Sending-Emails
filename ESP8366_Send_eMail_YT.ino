#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <base64.h>

const int     SMTP_PORT            = 465;
const char*   SMTP_SERVER          = "smtp.gmail.com";
String        error_message;
String        ServerResponse;
//################################################
const char*   ssid     = "your_wifi_ssid";                       // WIFI network name
const char*   password = "your_wifi_password";                   // WIFI network password
//################################################
String Senders_Login    = "your_email_user_account@gdomain.com";
String Senders_Password = "your_email_account_password";
String From;
//################################################
String To, Subject, Message, Login_base64, Passwrd_base64;

WiFiClientSecure client;
  
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); 
  Serial.print("Connecting to :"+String(ssid));
}

void loop() {
  To      = "recipients_email_address@domain.com";
  From    = "your_email_address@domain.com";
  Subject = "Test Message";
  Message = "This is a test message to demonstrate how to send an email from and ESP8266"; 
  SendMail(To, Subject, Message);
  if (error_message != "") Serial.println(error_message);
  delay(10000000);
}

void SendMail(String To, String Subject, String Message) {
  if (!client.connect(SMTP_SERVER, SMTP_PORT)) {
    error_message = "SMTP responded that it could not connect to the mail server";
    return;
  }
  if (ErrorWhileWaitingForSMTP_Response("220", 500)) {
    error_message = "SMTP responded with a Connection Error";
    return;
  }
  client.println("HELO server");
  if (ErrorWhileWaitingForSMTP_Response("250", 500)) {
    error_message = "SMTP responded with an Identification error";
    return;
  }
  client.println("AUTH LOGIN");
  WaitSMTPResponse(ServerResponse, 500);
  client.println(base64::encode(Senders_Login));
  WaitSMTPResponse(ServerResponse, 500);
  client.println(base64::encode(Senders_Password));;
  if (ErrorWhileWaitingForSMTP_Response("235", 500)) {
    error_message = "SMTP responded with an Authorisation error";
    return;
  }
  String mailFrom = "MAIL FROM: <" + String(From) + '>';
  client.println(mailFrom);
  WaitSMTPResponse(ServerResponse, 500);
  String recipient = "RCPT TO: <" + To + '>';
  client.println(recipient);
  WaitSMTPResponse(ServerResponse, 500);
  client.println("DATA");
  if (ErrorWhileWaitingForSMTP_Response("354", 500)) {
    error_message = "SMTP DATA error";
    return;
  }
  client.println("From: <" + String(From) + '>');
  client.println("To: <" + String(To) + '>');
  client.print("Subject: ");
  client.println(String(Subject));
  client.println("Mime-Version: 1.0");
  client.println("Content-Type: text/html; charset=\"UTF-8\"");
  client.println("Content-Transfer-Encoding: 7bit");
  client.println();
  String body = "<!DOCTYPE html><html lang=\"en\">" + Message + "</html>";
  client.println(body);
  client.println(".");
  if (ErrorWhileWaitingForSMTP_Response("250", 1000)) {
    error_message = "SMTP responded with a Message error";
    return;
  }
  client.println("QUIT");
  if (ErrorWhileWaitingForSMTP_Response("221", 1000)) {
    error_message = "SMTP responded with a QUIT error";
    return;
  }
  client.stop();
  Serial.println("Message Sent");
}

bool ErrorWhileWaitingForSMTP_Response(String Error_Code, int TimeOut) {
  int timer = millis();
  while (!client.available()) {
    if (millis() > (timer + TimeOut)) {
      error_message = "SMTP responsed that a Timeout occurred";
      return true;
    }
  }
  ServerResponse = client.readStringUntil('\n');
  if (ServerResponse.indexOf(Error_Code) == -1) return true;
  return false;
}

bool WaitSMTPResponse(String Error_Code, int TimeOut) {
  int timer = millis();
  while (!client.available()) {
    if (millis() > (timer + TimeOut)) {
      error_message = "SMTP responded that a Timeout occurred";
      return false;
    }
  }
  ServerResponse = client.readStringUntil('\n');
  if (ServerResponse.indexOf(Error_Code) == -1) return false;
  return true;
}

