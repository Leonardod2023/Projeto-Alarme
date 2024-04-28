#include <WiFi.h>
#include <WebServer.h>

#define BUTTON_PIN 2   // Pino onde o botão está conectado
#define BUZZER_PIN 12  // Pino onde o buzzer está conectado

const char* ssid = "Iphone";
const char* password = "12345678";

WebServer server(80);

bool buttonState = false; // Estado atual do botão
bool buzzerState = false; // Estado atual do buzzer

void setup() {
  Serial.begin(115200);
  
  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi..");
  }
  Serial.println("Conectado à rede Wi-Fi");
  
  // Define o pino do botão como entrada e o pino do buzzer como saída
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Define as rotas para o servidor web
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate); // Adiciona rota para atualização do estado do alarme
  server.begin();
  Serial.println("Servidor web iniciado");
  Serial.print("Endereço IP do servidor: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();

  // Verifica se houve uma mudança de estado do botão
  bool lastButtonState = buttonState;
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) { // Se o botão foi pressionado
      buzzerState = !buzzerState; // Inverte o estado do buzzer
    }
  }

  // Atualiza o estado do buzzer
  digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
}

void handleRoot() {
  String page = "<!DOCTYPE html>\
<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>Estado do Alarme</title>\
<style>\
body {\
  background-color: #222;\
  color: white;\
  font-family: Arial, sans-serif;\
  text-align: center;\
}\
.container {\
  display: flex;\
  justify-content: center;\
  align-items: center;\
  height: 100vh;\
}\
</style>\
</head>\
<body>\
<div class='container'>\
  <div>\
    <h1>Estado do Alarme</h1>\
    <p>O ALARME ESTA ";
  page += (buzzerState ? "LIGADO" : "DESLIGADO");
  page += ".</p>\
    <form action='/update' method='POST'>\
      Senha: <input type='password' name='senha'><br>\
      <button type='submit'>Enviar</button>\
    </form>\
  </div>\
</div>\
</body>\
</html>";

  server.send(200, "text/html", page);
}

void handleUpdate() {
  if (server.hasArg("senha")) {
    String senhaDigitada = server.arg("senha");
    // Verifica se a senha está correta
    if (senhaDigitada == "12345") {
      server.send(200, "text/html", "<center><h1 style='color: green;'>Senha correta. Alarme atualizado.</h1></center>");
      buzzerState = !buzzerState; // Inverte o estado do buzzer
    } else {
      server.send(401, "text/html", "<center><h1 style='color: red;'>Senha incorreta.</h1></center>");
    }
  } else {
    server.send(400, "text/html", "<center><h1 style='color: red;'>Erro: Senha não fornecida.</h1></center>");
  }
}
