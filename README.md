# 🌡️ IoT Temperature Monitoring & Device Control System

> Sistem monitoring suhu dan kelembaban real-time menggunakan ESP8266 dengan kontrol perangkat jarak jauh melalui Blynk dan notifikasi WhatsApp via Twilio

[![C++](https://img.shields.io/badge/C++-100%25-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://github.com/bagaspng/Monitoring-Suhu-Kontrol-Perangkat-dengan-ESP8266-Blynk-dan-WhatsApp-Twilio-)
[![ESP8266](https://img.shields.io/badge/Platform-ESP8266-red?style=flat-square&logo=espressif&logoColor=white)](https://www.espressif.com/)
[![Arduino](https://img.shields.io/badge/IDE-Arduino-00979D?style=flat-square&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![IoT](https://img.shields.io/badge/Category-IoT-success?style=flat-square&logo=internetofthings&logoColor=white)](https://github.com/bagaspng/Monitoring-Suhu-Kontrol-Perangkat-dengan-ESP8266-Blynk-dan-WhatsApp-Twilio-)
[![Blynk](https://img.shields.io/badge/Platform-Blynk-blue?style=flat-square&logo=blynk&logoColor=white)](https://blynk.io/)
[![Twilio](https://img.shields.io/badge/Service-Twilio-F22F46?style=flat-square&logo=twilio&logoColor=white)](https://www.twilio.com/)

## 📋 Deskripsi

Sistem IoT ini mengintegrasikan **ESP8266**, **sensor DHT22**, **platform Blynk**, dan **Twilio WhatsApp API** untuk menciptakan solusi monitoring suhu dan kelembaban yang komprehensif.  Sistem dapat mengontrol perangkat listrik (kulkas, TV) secara remote dan mengirim notifikasi WhatsApp otomatis ketika suhu melebihi ambang batas yang ditentukan.

## ✨ Fitur Utama

### 🌡️ **Temperature & Humidity Monitoring**
- 📊 **Real-time Monitoring** - Pembacaan sensor DHT22 setiap 2 detik
- 📈 **Data Visualization** - Dashboard Blynk dengan grafik real-time
- 🎯 **Threshold Alert** - Notifikasi otomatis saat suhu > 27°C
- 💡 **Visual Indicators** - LED indikator dengan 3 level suhu

### 📱 **Remote Control Features**
- 🔌 **Device Control** - Kontrol kulkas dan TV via Blynk app
- 📲 **Mobile Dashboard** - Interface user-friendly di smartphone
- 🌐 **Web Dashboard** - Akses melalui web browser
- ⚡ **Real-time Control** - Respons kontrol instan

### 📞 **WhatsApp Notifications**
- 📤 **Auto Notifications** - WhatsApp alert otomatis via Twilio
- ⏰ **Cooldown System** - Mencegah spam notifikasi (5 menit interval)
- 📋 **Status Reports** - Laporan lengkap suhu, kelembaban, dan status perangkat
- 🔒 **Secure Authentication** - Twilio API dengan Basic Auth


