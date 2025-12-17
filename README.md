# CPU-Simulation

Bu depo, eğitim/öğrenme amaçlı basit bir CPU simülasyonu içeren kaynakları barındırır.

## İçerik
- `cpu.cpp` — Proje ana kaynağı (C++). CPU simülasyonunun/deneyinin mantığını içerir.
- `data.txt` — Örnek veri/girdi dosyası (varsa). Programın hangi formatta veri beklediğini buraya koyabilirsiniz.

> Not: `cpu.cpp` dosyasının nasıl davrandığı (komut satırı argümanları mı, stdin mi vb.) kodu açıp kontrol edilmelidir; aşağıdaki talimatlar yaygın senaryolara göre verilmiştir.

## Gereksinimler
- macOS (veya başka bir UNIX benzeri ortam)
- C++ derleyicisi (ör. `g++`)

## Derleme
Aşağıdaki komutla derleyebilirsiniz:

```bash
# C++17 ile derleme
g++ -std=c++17 -O2 cpu.cpp -o cpu
```

## Çalıştırma
Eğer program girişini `stdin` üzerinden okuyor ise:

```bash
./cpu < data.txt
```

Eğer program bir dosya adı argümanı bekliyorsa:

```bash
./cpu data.txt
```

Program farklı bir argüman bekliyorsa veya komut satırı argümanları varsa, `cpu.cpp` içindeki `main` fonksiyonunu kontrol ederek doğru kullanım şeklini belirleyin.

## Örnek
`data.txt` içinde örnek bir girdi varsa, yukarıdaki `./cpu < data.txt` komutu programı çalıştırıp çıktı verecektir. Çıktı formatı `cpu.cpp` içeriğine bağlıdır.

## Katkıda Bulunma
Katkıda bulunmak isterseniz:
1. Yeni bir branch açın.
2. Değişikliklerinizi ekleyip commit yapın.
3. Pull request gönderin.




