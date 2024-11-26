//-----------------------------------------------------------------------------------------------------------------------------------------
// Área de Inclusão de arquivos.

#include <Arduino.h>                                                    // Para usar a framework do Arduino
#include <WiFi.h>                                                       // Para acesso ao WiFi
#include <HTTPClient.h>                                                 // Para criar um cliente HTTP
#include <UrlEncode.h>                                                  // Para enviar a mensagem via post deve ser no formato URL

//-----------------------------------------------------------------------------------------------------------------------------------------
// Definições de Hardware

#define Sensor  14                                                     // Sensor digital ligado ao GPIO 14
#define Led1    2                                                      // LED indicador de alarme (ligado ao GPIO 2)
#define Buzzer  32                                                     // Buzzer ligado ao GPIO 32
#define LedR    25                                                     // LED RGB (Vermelho - GPIO 25)
#define LedG    26                                                     // LED RGB (Verde - GPIO 26)
#define LedB    27                                                     // LED RGB (Azul - GPIO 27)
#define LedWiFi 4                                                      // LED WiFi (indica conexão ao WiFi)

//-----------------------------------------------------------------------------------------------------------------------------------------
// Protótipos de função

void WhatsAppMessages(String message);                                  // Função que envia uma mensagem para o WhatsApp
void reconectarWiFi();                                                  // Função para reconectar ao WiFi, se necessário
void alarmeVisualSonoro();                                              

//-----------------------------------------------------------------------------------------------------------------------------------------
// Dados da sua WiFi.

const char* SSID = "DGNETVIVO";                                         
const char* PASSWORD = "necoeco523232";                                

//-----------------------------------------------------------------------------------------------------------------------------------------
// Dados do celular e chave do WhatsApp

bool flag = 0;                                                          // Flag para uso na lógica do sensor
String phoneNumber = "+5512996548337";                                  // Meu número de celular com formato internacional
String apiKey = "9535548";                                              // A chave que o bot do WhatsApp enviou

unsigned long ultimaMensagem = 0;                                       // Controle de tempo para evitar mensagens repetidas
const unsigned long intervaloMensagens = 10000;                        // Tempo mínimo entre mensagens (30 segundos)

//-----------------------------------------------------------------------------------------------------------------------------------------

void setup() {
    pinMode(Sensor, INPUT_PULLUP);                                      // Configura pino do sensor como entrada e em pull-up
    pinMode(Led1, OUTPUT);                                              // Configura pino do LED indicador como saída
    pinMode(Buzzer, OUTPUT);                                            // Configura pino do Buzzer como saída
    pinMode(LedR, OUTPUT);                                              // Configura pino do LED RGB Vermelho como saída
    pinMode(LedG, OUTPUT);                                              // Configura pino do LED RGB Verde como saída
    pinMode(LedB, OUTPUT);                                              // Configura pino do LED RGB Azul como saída
    pinMode(LedWiFi, OUTPUT);                                           // Configura pino do LED WiFi como saída
    Serial.begin(115200);                                               // Inicia a serial com 115200 bps
    WiFi.begin(SSID, PASSWORD);                                         // Inicia o WiFi com o nome da rede e a senha
    Serial.println("Conectando ao WiFi...");

    while (WiFi.status() != WL_CONNECTED) {                             // Aguarda a conexão
        delay(500);                                                     // Espera 500ms
        Serial.print(".");                                              // Vai escrevendo ...
    }
    Serial.println("\nConectado no IP: ");
    Serial.println(WiFi.localIP());                                     // Mostra o IP no qual foi conectado
    digitalWrite(LedWiFi, HIGH);                                        // Indica que o WiFi está conectado
    WhatsAppMessages("Sistema iniciado e conectado ao WiFi!");         // Envia mensagem inicial ao WhatsApp
}

//-----------------------------------------------------------------------------------------------------------------------------------------

void loop() {
    reconectarWiFi();                                                   // Verifica e reconecta ao WiFi, se necessário
    int Porta = digitalRead(Sensor);                                    // Faz a leitura do sensor

    if (Porta == HIGH) {                                                // Porta acionada?
        if (flag) {                                                     // Primeira vez acionado?
            if (millis() - ultimaMensagem > intervaloMensagens) {       // Verifica se já passou o intervalo mínimo
                Serial.println("ALARME ACIONADO!");
                WhatsAppMessages("ALARME ACIONADO!");
                digitalWrite(Led1, HIGH);                               // Liga o Led1 para indicar alarme
                alarmeVisualSonoro();                                   // Ativa o LED RGB e o buzzer
                ultimaMensagem = millis();                              // Atualiza o tempo da última mensagem
            }
            flag = 0;                                                   // Zera o flag
        }
    } else {
        if (flag == 0) {                                                // Sensor voltou ao estado normal
            Serial.println("TUDO NORMAL!");
            digitalWrite(Led1, LOW);                                    // Desliga o Led1
            digitalWrite(Buzzer, LOW);                                  // Desliga o Buzzer
            flag = 1;                                                   // Reinicia o flag
        }
    }
    delay(250);                                                         // Delay para evitar leituras excessivas
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Funções auxiliares

void WhatsAppMessages(String message) {
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
    HTTPClient http;
    http.begin(url);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    uint16_t httpResponseCode = http.POST(url);

    if (httpResponseCode == 200) {
        Serial.println("Mensagem enviada com sucesso!");
    } else {
        Serial.println("Erro ao tentar enviar a mensagem!");
        Serial.print("Código HTTP: ");
        Serial.println(httpResponseCode);
    }
    http.end();
}

void reconectarWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LedWiFi, LOW);                                     // Apaga o LED de WiFi
        Serial.println("WiFi desconectado. Tentando reconectar...");
        WiFi.begin(SSID, PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nWiFi reconectado!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LedWiFi, HIGH);                                    // Liga o LED de WiFi
    }
}

void alarmeVisualSonoro() {
    for (int i = 0; i < 10; i++) {                                      // Faz o LED piscar 10 vezes entre azul e amarelo
        // Liga LED Azul
        digitalWrite(LedR, LOW);
        digitalWrite(LedG, LOW);
        digitalWrite(LedB, HIGH);

        // Liga Buzzer
        digitalWrite(Buzzer, HIGH);
        delay(250);

        // Liga LED Amarelo
        digitalWrite(LedR, HIGH);
        digitalWrite(LedG, HIGH);
        digitalWrite(LedB, LOW);

        // Desliga Buzzer
        digitalWrite(Buzzer, LOW);
        delay(250);
    }
}
