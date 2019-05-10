#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

//Configuracoes MQTT e Wi-Fi

#define ledAmarelo 32 
#define ledVermelho 12
const char* ssid = "Nome_da_sua_rede";
const char* password = "Sua_senha;
const char* broker = "Seu_Broker";
const int port = PORT;
const char *ID = "Fechadura";
const char *topico = "DAMM/Metrologia";
#define SS_PIN 21
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
 Serial.println("\nConectando com ");
 Serial.println(ssid);
 
 WiFi.begin(ssid, password);   
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando com Wi-Fi...");
}
 
Serial.println("Conectado!");
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
digitalWrite(ledAmarelo, HIGH);
delay(3000);
digitalWrite(ledAmarelo, LOW);
}


void reconectando ()
{
// Loop ate reconectar
  while (!client.connected()) {
    Serial.print("Tentando uma conexao MQTT...");
    // Tentando se conectar
    if (client.connect(ID)) {
      Serial.println("Conectado");
      Serial.print("Publicando no topico: ");
      Serial.println(topico);
      Serial.println('\n');

    } else {
      Serial.println(" tente novamente em 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  
  Serial.begin(115200); // Start serial communication at 115200 baud
  pinMode(ledAmarelo,OUTPUT);
  pinMode(ledVermelho,OUTPUT);
  delay(100);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  setup_wifi(); // Connect to network
  client.setServer(broker, port);
  Serial.println("Aproxime seu cartao RFID...");
  Serial.println();
  Serial.println("--------------------------- Sistema Fechadura ---------------------------");
}
  



void loop() {
  if (!client.connected())  // Reconnect if connection is lost
  {
    reconectando();
  }
  client.loop();
 
if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  
  content.toUpperCase();
  if (content.substring(1) == "33 F9 B8 C3") //change here the UID of the card/cards that you want to give access
  {
       
    Serial.println("--------------------------- ACESSO ---------------------------");
    Serial.println("PERMITIDO");
    Serial.println("Bem vinda, Victoria");
    client.publish(topico,"Victoria entrou na sala");
    
    Serial.println("--------------------------------------------------------------");
    digitalWrite(ledAmarelo, HIGH);
    Serial.println();
    delay(3000);
    digitalWrite(ledAmarelo, LOW);
  }
 
 else   {
    Serial.println("--------------------------- ACESSO ---------------------------");
    client.publish(topico,"Alguem nao autorizado tentou entrar");
    Serial.println("NEGADO");
    Serial.println("--------------------------------------------------------------");
    digitalWrite(ledVermelho, HIGH);
    delay(3000);
    digitalWrite(ledVermelho, LOW); 
  }

}
