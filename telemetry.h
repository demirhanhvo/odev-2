//telemetry.h dosyası örnek içeriği 

#ifndef TELEMETRY_H
#define TELEMETRY_H

#define MAX_KAYIT 100

//Fonksiyon Prototipleri (imzaları)
void sistemi_kur(float baslangic_sicaklik, int baslangic_sarj);
void gaza_bas();
void rejen_fren();
void telemetri_ve_istatistikleri_yazdir();
void sistemi_kapat();

#endif