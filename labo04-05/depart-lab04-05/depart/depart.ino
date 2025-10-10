#include <MeAuriga.h>

bool debugMode = true;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
}

// Événement qui se déclenche automatiquement lorsqu'il y a
// réception de données via le port série (Bluetooth ou Moniteur)
void serialEvent()
{
  static String receivedData = "";

  if (!Serial.available()) return;

  receivedData = Serial.readStringUntil('\n');
  parseData(receivedData);
}

void parseData(String& receivedData)
{
  bool isFromBLE = false;

  if (receivedData.length() >= 2)
  {
    // Vérifier si les deux premiers octets sont 0xFF55 (BLE) --> (Header)
    if ((uint8_t)receivedData[0] == 0xFF && (uint8_t)receivedData[1] == 0x55)
    {
      // Supprimer les deux premiers octets du header (0xFF55)
      receivedData.remove(0, 2);
      isFromBLE = true;
    }
    // Vérifier si les deux premiers caractères sont "!!" (Moniteur Série)
    else if (receivedData.startsWith("!!"))
    {
      // Supprimer les deux premiers octets du header (!!)
      receivedData.remove(0, 2);
    }
    else
    {
      // En-tête non reconnue
      Serial.print(F("Données non reconnues : "));
      Serial.println(receivedData);
      return;
    }
  }
  else
  {
    Serial.print(F("Données trop courtes : "));
    Serial.println(receivedData);
    return;
  }

  if (debugMode)
  {
    Serial.print(F("Reçu du "));
    Serial.print(isFromBLE ? F("BLE") : F("Moniteur Série"));
    Serial.print(" : ");
    Serial.println(receivedData);
  }

  int index = receivedData.indexOf(',');
  if (index == -1)
  {
    gererCommandeSimple(receivedData);
  }
  else
  {
    String action = receivedData.substring(0, index);
    String parametre = receivedData.substring(index + 1);
    gererCommandeComposee(action, parametre);
  }
}

void gererCommandeSimple(const String& cmd)
{
  // À supprimer
  Serial.print("Commande simple : ");
  Serial.println(cmd);
}

void gererCommandeComposee(const String& action, const String& parametre)
{
  // À supprimer
  Serial.print("Commande composée : action -> ");
  Serial.print(action);
  Serial.print(" - paramètre -> ");
  Serial.println(parametre);
}