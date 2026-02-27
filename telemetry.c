#include "telemetry.h"
#include <stdio.h>
#include <stdlib.h> // rand() ve exit() fonksiyonları için

static float s_hiz = 0.0f;
static float s_batarya_sicaklik = 0.0f;
static float s_motor_sicaklik = 0.0f;
static int   s_sarj = 0;

// İstatistik Dizileri ve Sayaçları
static float hizlanma_kayitlari[MAX_KAYIT];
static int   hizlanma_sayaci = 0;

static float yavaslama_kayitlari[MAX_KAYIT];
static int   yavaslama_sayaci = 0;

static float rejen_kayitlari[MAX_KAYIT];
static int   rejen_sayaci = 0;


void sistemi_kur(float baslangic_sicaklik, int baslangic_sarj) {
    // aynı ortam sıcaklığıyla başlar
    s_batarya_sicaklik = baslangic_sicaklik;
    s_motor_sicaklik = baslangic_sicaklik;
    s_sarj = baslangic_sarj;
    s_hiz = 0.0f;
}

void gaza_bas() {
    // Batarya bittiyse eylem iptal
    if (s_sarj < 2) {
        printf("[UYARI] Batarya Tükendi! Araç hareket ettirilemiyor.\n");
        return;
    }

    // 2.0 ile 10.0 arası rastgele hızlanma değeri üretimi
    float artis = (rand() % 81 + 20) / 10.0f;

    // Etkileri uygula
    s_sarj -= 2;
    s_motor_sicaklik += 5.0f;
    s_batarya_sicaklik += 2.0f;

    if (s_motor_sicaklik >= 90.0f || s_batarya_sicaklik >= 70.0f) {
        printf("\nKRİTİK HATA: Aşırı Isınma! (Motor: %.1f C, Batarya: %.1f C)\n", s_motor_sicaklik, s_batarya_sicaklik);
        printf("Sistem acil olarak kapatılıyor...\n");
        exit(1); 
    }

    // 3. Sınır Kontrolü: Maksimum Hız Sınırı (70 km/s)
    float gercek_artis = artis;
    if (s_hiz + artis > 70.0f) {
        gercek_artis = 70.0f - s_hiz; 
        s_hiz = 70.0f;
    } else {
        s_hiz += artis;
    }

    // Diziye kaydet (Dizi taşmasını engelle)
    if (hizlanma_sayaci < MAX_KAYIT) {
        hizlanma_kayitlari[hizlanma_sayaci++] = gercek_artis;
    }

    printf("[BİLGİ] Gaza basıldı. Araç %.1f km/s hızlandı. Motor: %.1f C, Batarya: %.1f C\n", 
           gercek_artis, s_motor_sicaklik, s_batarya_sicaklik);
}

void frene_bas() {
    if (s_hiz <= 0.0f) {
        printf("[UYARI] Araç zaten duruyor!\n");
        return;
    }

    float dusus = (rand() % 101 + 50) / 10.0f;

    // Etkileri uygula
    s_motor_sicaklik -= 3.0f;
    s_batarya_sicaklik -= 1.0f;

    // 2. Sınır Kontrolü: Soğuma Sınırı (Ortam sıcaklığı 20 derece alt limiti)
    if (s_motor_sicaklik < 20.0f) s_motor_sicaklik = 20.0f;
    if (s_batarya_sicaklik < 20.0f) s_batarya_sicaklik = 20.0f;

    // 3. Sınır Kontrolü: Negatif Hız Koruması
    float gercek_dusus = dusus;
    if (s_hiz - dusus < 0.0f) {
        gercek_dusus = s_hiz; // Sadece sıfıra düşürecek kadar olan miktarı kaydet
        s_hiz = 0.0f;
    } else {
        s_hiz -= dusus;
    }

    if (yavaslama_sayaci < MAX_KAYIT) {
        yavaslama_kayitlari[yavaslama_sayaci++] = gercek_dusus;
    }

    printf("[BİLGİ] Frene basıldı. Araç %.1f km/s yavaşladı.\n", gercek_dusus);
}

void rejen_fren() {
    if (s_hiz <= 0.0f) {
        printf("[UYARI] Araç zaten duruyor!\n");
        return;
    }

    float dusus = (rand() % 81 + 20) / 10.0f;

    s_sarj += 1;
    if (s_sarj > 100) s_sarj = 100; 

    s_motor_sicaklik -= 2.0f;
    if (s_motor_sicaklik < 20.0f) s_motor_sicaklik = 20.0f;

    s_batarya_sicaklik += 1.0f;
    
    // Batarya ısındığı için aşırı ısınma kontrolü
    if (s_batarya_sicaklik >= 70.0f) {
        printf("\nKRİTİK HATA: Aşırı Isınma! (Batarya: %.1f C)\n", s_batarya_sicaklik);
        printf("Sistem acil olarak kapatılıyor...\n");
        exit(1);
    }

    float gercek_dusus = dusus;
    if (s_hiz - dusus < 0.0f) {
        gercek_dusus = s_hiz;
        s_hiz = 0.0f;
    } else {
        s_hiz -= dusus;
    }

    if (rejen_sayaci < MAX_KAYIT) {
        rejen_kayitlari[rejen_sayaci++] = gercek_dusus;
    }

    printf("[BİLGİ] Rejeneratif fren devrede. Araç %.1f km/s yavaşladı. Batarya şarj oluyor.\n", gercek_dusus);
}

void telemetri_ve_istatistik_yazdir() {
    printf("\n=== ANLIK TELEMETRİ ===\n");
    printf("Güncel Hız: %.1f km/s\n", s_hiz);
    printf("Batarya: %%%d\n", s_sarj);
    printf("Motor Sıcaklığı: %.1f C\n", s_motor_sicaklik);
    printf("Batarya Sıcaklığı: %.1f C\n", s_batarya_sicaklik);

    printf("\n=== SÜRÜŞ İSTATİSTİKLERİ ===\n");

    // Gaza basma ortalaması
    float toplam = 0;
    for (int i = 0; i < hizlanma_sayaci; i++) toplam += hizlanma_kayitlari[i];
    float ort = (hizlanma_sayaci > 0) ? (toplam / hizlanma_sayaci) : 0;
    printf("Gaza Basma Sayısı: %d (Ortalama Artış: %.2f km/s)\n", hizlanma_sayaci, ort);

    // Frene basma ortalaması
    toplam = 0;
    for (int i = 0; i < yavaslama_sayaci; i++) toplam += yavaslama_kayitlari[i];
    ort = (yavaslama_sayaci > 0) ? (toplam / yavaslama_sayaci) : 0;
    printf("Frene Basma Sayısı: %d (Ortalama Düşüş: %.2f km/s)\n", yavaslama_sayaci, ort);

    // Rejen fren ortalaması
    toplam = 0;
    for (int i = 0; i < rejen_sayaci; i++) toplam += rejen_kayitlari[i];
    ort = (rejen_sayaci > 0) ? (toplam / rejen_sayaci) : 0;
    printf("Rejeneratif Frenleme Sayısı: %d (Ortalama Düşüş: %.2f km/s)\n", rejen_sayaci, ort);
    printf("============================\n");
}

void sistemi_kapat() {
    printf("\n[BİLGİ] Sistem kapatılıyor...\n");
    telemetri_ve_istatistik_yazdir();
    printf("\n[BİLGİ] Motor güvenli bir şekilde kapatıldı. İyi günler!\n");
}