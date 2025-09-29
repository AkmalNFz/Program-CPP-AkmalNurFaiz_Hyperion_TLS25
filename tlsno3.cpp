#include <iostream>
#include <string>
using namespace std;

string lampu(int t) {
    int hijau = 20, kuning = 3, merah = 80;
    int total = hijau + kuning + merah; // 103 detik

    // Cari offset (supaya t=45 jatuh di kuning)
    int offset = 0;
    for (int o = 0; o < total; o++) {
        int pos = (45 + o) % total;
        if (pos >= 20 && pos < 23) {
            offset = o;
            break;
        }
    }

    int pos = (t + offset) % total;
    if (pos < hijau) return "Hijau";
    else if (pos < hijau + kuning) return "Kuning";
    else return "Merah";
}

int main() {
    int t;
    cout << "Masukkan detik ke-: ";
    cin >> t;
    cout << "Lampu pada detik ke-" << t << " adalah: " << lampu(t) << endl;
    return 0;
}
