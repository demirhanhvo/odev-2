#include <stdio.h>
#include <stdlib.h> // srand() için gerekli [cite: 58]
#include <time.h>   // time() için gerekli [cite: 58]
#include "telemetry.h" // Fonksiyon prototiplerini bağlar [cite: 42]

int main() {
    // Rastgele sayı üreticiyi saat verisiyle başlat [cite: 58]
    srand(time(NULL));

    // Değişken Tanımlamaları
    float battery_temp;
    int charge, break_state;
    char door_state;
    int choice = 0; // Menü seçimi için

    printf("--- ELEKTROMOBIL GUVENLIK KONTROLU ---\n");

    // --- AŞAMA 1: GÜVENLİK KONTROLLERİ (Hafta 1) [cite: 54] ---
    printf("Batarya sicakligini giriniz: ");
    scanf(" %f", &battery_temp);

    if (battery_temp >= 60.0f) {
        printf("Batarya sicakligi cok yuksek. Motoru calistirmak sistem tarafindan kitlendi.\n");
        return 0; // Sistem kilitlendiği için program sonlanır
    }

    printf("Sarj yuzdesini giriniz: ");
    scanf(" %d", &charge);

    if (charge > 100 || charge < 0) {
        printf("KRITIK HATA: Sarj seviyesi %%d. Lutfen sensor verilerini kontrol ediniz.\n", charge);
        return 0;
    }

    printf("Fren pedalina basiyorsaniz '1', basmiyorsaniz '0' tuslayiniz: ");
    scanf(" %d", &break_state);

    if (break_state != 0 && break_state != 1) {
        printf("Lutfen dogru bir deger giriniz.\n");
        return 0;
    }

    printf("Kapi durumu (A/K): ");
    scanf(" %c", &door_state);

    if (door_state != 'A' && door_state != 'K') {
        printf("Lutfen kapi durumunu belirtilen sekilde giriniz.\n");
        return 0;
    }

    // Motor başlatma onayı kontrolü [cite: 54]
    if (charge >= 20 && charge <= 100 && battery_temp < 60 && break_state == 1 && door_state == 'K') {
        printf("\nSONUC: BASARILI: SISTEM HAZIR. MOTOR BASLATILIYOR...\n");
        
        // --- AŞAMA 2: SİMÜLASYON KURULUMU  ---
        // Kullanıcıdan alınan başlangıç değerlerini telemetri modülüne gönder 
        sistemi_kur(battery_temp, charge);

        // --- AŞAMA 3: SONSUZ SİMÜLASYON DÖNGÜSÜ [cite: 6] ---
        while (1) {
            printf("\nTUFA ELEKTROMOBIL SURUS SIMULASYONU\n");
            printf("1. Gaza Bas\n");
            printf("2. Frene Bas\n");
            printf("3. Rejeneratif Frenleme Yap\n");
            printf("4. Anlik Telemetri ve Istatistikleri Oku\n");
            printf("5. Sistemi Kapat\n");
            printf("Seciminiz: ");

            // Terminal çökmesini (harf girilmesi durumunu) engelleme 
            if (scanf("%d", &choice) != 1) {
                printf("\n[HATA] Gecersiz giris! Lutfen 1-5 arasinda bir sayi giriniz.\n");
                while (getchar() != '\n'); // Buffer'ı temizle [cite: 94]
                continue;
            }

            // Seçime göre ilgili fonksiyonu tetikle [cite: 21, 63]
            switch (choice) {
                case 1:
                    gaza_bas();
                    break;
                case 2:
                    frene_bas();
                    break;
                case 3:
                    rejen_fren();
                    break;
                case 4:
                    telemetri_ve_istatistik_yazdir();
                    break;
                case 5:
                    sistemi_kapat(); // Son istatistikleri yazdırır [cite: 74]
                    return 0; // Döngüyü kırarak programı sonlandırır
                default:
                    printf("Gecersiz secim! Lutfen menudeki rakamlardan birini giriniz.\n");
                    break;
            }
        }
    } else {
        // Güvenlik kontrolleri başarısız durumu
        if (charge < 20) printf("Sarj seviyesi dusuk. Optimum surus icin bataryayi sarj ediniz.\n");
        if (break_state == 0) printf("Fren pedali basili degil. Guvenliginiz icin fren pedalina basarak calistiriniz.\n");
        if (door_state == 'A') printf("Kapilar acik. Guvenliginiz icin kapilari kapatip calistirin.\n");
        
        printf("Motor baslatilmadi.\n");
    }

    return 0;
}