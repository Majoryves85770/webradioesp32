# webradioesp32
radio web wifi 
# WebRadio ESP32

Projet de WebRadio basé sur un ESP32, utilisant un décodeur VS1053, un affichage graphique et un système de sélection de stations.  
Le code est structuré en C++ avec séparation en fichiers `.cpp` et `.h` pour une meilleure organisation.
Le tout est assemblé dans une radio FM d'une marque bien connue , à peine modifiée 

<img width="382" height="244" alt="image" src="https://github.com/user-attachments/assets/b31a4a50-7b73-4e2e-88ad-1ca2f33fd1b2" />
<img width="518" height="291" alt="image" src="https://github.com/user-attachments/assets/2c48cadd-54d5-4475-961b-4955d48a6b4a" />



des touches tactiles afin de changer de stations , l'affichage du logo de la station sélectionnée et un équalizer en dessous du logo. Un bouton marche arrêt et l'ampli LOSC 2X 5W
---

## 🎧 Fonctionnalités

- Lecture de flux radio en ligne (HTTP/MP3)
- Gestion du décodeur audio VS1053
- Affichage graphique (logos, FFT simulée, infos station)
- Boutons tactiles pour changer de station
- Organisation propre du code en `src/` et `include/`

---

## 📂 Structure du projet
webradioesp32/
├── src/
│   ├── main.cpp
│   ├── logos.h
│   └── vs1053_ext.cpp
├── include/
│   ├── vs1053_ext.h
│   └── vs1053b-patches-flac.h
├── .gitignore
├── LICENSE
└── README.md

---

## 🔧 Matériel utilisé

- ESP32 vromm 32
- <img width="93" height="179" alt="image" src="https://github.com/user-attachments/assets/0f82a07b-2c26-4e80-b704-85432944f2a3" />

- Module VS1053 (MP3/AAC/FLAC)
- <img width="311" height="307" alt="image" src="https://github.com/user-attachments/assets/cda1297a-be99-45c3-87f1-eecdcb938083" />

- Écran TFT 2.8 tactile série // préférable aux écrans tft parallèles car trops compliqués à cabler // reconnaissables à la série de pin sur un coté !
- <img width="287" height="346" alt="image" src="https://github.com/user-attachments/assets/f0b0387f-2840-4b57-87a8-ef1150445f46" />

- ampli LOSC 2x 5W
<img width="135" height="124" alt="image" src="https://github.com/user-attachments/assets/61dddeaf-c3a7-4a55-b7c2-fa383d9dd269" />

- Alimentation 5V


---

## 📡 Dépendances Arduino

Installer les bibliothèques suivantes :

- ESP32 Core  : pilote ch340 
- VS1053 Library  : VS1053_ext.h // ATTENTION joindre au fichier initial (dans le même répertoire) : vs1053_ext.cpp , vs1053_ext.h, VS1053b-patches.flac
- WiFiClient / HTTPClient  
- TFT eSPI 

---

## 🚀 Installation

1. Cloner le dépôt  
2. Ouvrir le dossier dans Arduino IDE ou VS Code  
3. Charger les bibliothèques nécessaires  
4. Compiler et téléverser sur l’ESP32  
5. Modifier les paramètres WiFi dans `main.cpp`

---

## 📻 Ajouter vos stations

Dans `main.cpp`, modifier le tableau :

```cpp
const char* stations[] = {
    "http://flux1.mp3",
    "http://flux2.mp3",
    "http://flux3.mp3"
};
Licence
Projet sous licence MIT.
Vous êtes libre de l’utiliser, modifier et redistribuer.

---

#
