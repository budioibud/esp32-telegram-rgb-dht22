# esp32-telegram-rgb-dht22

Sistem IoT sederhana menggunakan **ESP32** untuk mengirim data **suhu dan kelembapan** ke **Telegram**, sekaligus mengontrol **lampu RGB** secara real-time melalui perintah chat.

## 🧰 Komponen yang Digunakan

- ESP32 Dev Module
- Sensor DHT22 (suhu & kelembapan)
- LED RGB Common Anode
- Resistor 220 ohm (x3)
- Kabel jumper dan breadboard
- Akses WiFi dan akun Telegram

## ⚙️ Cara Kerja

1. ESP32 membaca data suhu dan kelembapan dari sensor DHT22.
2. Data dikirim ke chat Telegram setiap 10 detik melalui bot.
3. Pengguna dapat mengirim perintah seperti `/on`, `/off`, `/red_on`, dll. untuk mengontrol warna RGB LED.
4. Semua perintah ditangani langsung oleh ESP32.

## 🔌 Skema Koneksi

- **DHT22** → Pin 4
- **LED RGB**:
  - Merah → Pin 27
  - Hijau → Pin 26
  - Biru → Pin 25
  - Anode → 5V (dengan resistor)

## 💬 Perintah Telegram

- `/start` – Menampilkan daftar perintah
- `/on` – Menyalakan semua warna (putih)
- `/off` – Mematikan semua warna
- `/red_on`, `/green_on`, `/blue_on` – Menyalakan warna tertentu
- `/red_off`, `/green_off`, `/blue_off` – Mematikan warna tertentu

## 🔐 Token Bot Telegram

1. Cari BotFather di Telegram.
2. Buat bot baru dengan `/newbot`.
3. Simpan token yang diberikan dan masukkan ke dalam kode di bagian `#define BOT_TOKEN "YOUR_TOKEN_HERE"`.

## 🚀 Upload ke ESP32

1. Pastikan board ESP32 sudah terpasang di Arduino IDE.
2. Ganti `BOT_TOKEN` dan `WIFI_SSID` / `WIFI_PASSWORD` di dalam kode.
3. Upload seperti biasa ke ESP32.

## 📷 Demo Video

📺 Tonton demo lengkap di sini:  
👉 [Kontrol Lampu & Kirim Suhu ke Telegram dengan ESP32](https://ai.invideo.io/ai-video?video=kontrol-lampu-kirim-data-suhu-ke-telegram-dengan-esp32-gildvt)

## 📄 Lisensi

Proyek ini menggunakan lisensi **MIT** – silakan digunakan atau dimodifikasi dengan bebas!

---

