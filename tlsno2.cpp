#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
using namespace std;

string user_defined_reverse(const string &s) {
    string r;
    r.resize(s.size());
    for (size_t i = 0; i < s.size(); ++i) r[i] = s[s.size() - 1 - i];
    return r;
}

bool is_vowel(char c) {
    char lc = (char)tolower((unsigned char)c);
    return lc=='a' || lc=='e' || lc=='i' || lc=='o' || lc=='u';
}


string encrypt_machine(const string &word) {
    if (word.empty()) return "";
    char first = word[0];
    int ascii_first = (int)(unsigned char)first; 
    string consonants;
    for (char c : word) {
        if (!is_vowel(c)) consonants.push_back(c);
    }
    string rev = user_defined_reverse(consonants);
    int len_rev = (int)rev.size();
    int insert_pos = (len_rev + 1) / 2; 

    ostringstream oss;
    oss << ascii_first;
    string ascii_str = oss.str();

    string sandi;
    sandi.reserve(rev.size() + ascii_str.size());
    sandi = rev.substr(0, insert_pos) + ascii_str + rev.substr(insert_pos);
    return sandi;
}
bool parse_sandi(const string &sandi, char &out_first_letter, string &out_consonant_skeleton, string &error_msg) {
    int n = (int)sandi.size();
    if (n == 0) { error_msg = "Sandi kosong."; return false; }
    vector<pair<int,int>> blocks; 
    int i = 0;
    while (i < n) {
        if (isdigit((unsigned char)sandi[i])) {
            int j = i;
            while (j < n && isdigit((unsigned char)sandi[j])) ++j;
            blocks.push_back({i,j});
            i = j;
        } else ++i;
    }
    if (blocks.empty()) { error_msg = "Tidak ditemukan blok angka (ASCII) pada sandi."; return false; }

    int mid = n/2;
    int bestIdx = 0;
    int bestDist = 1<<30;
    for (size_t k=0;k<blocks.size();++k) {
        int s = blocks[k].first, e = blocks[k].second; 
        int center = (s + e - 1) / 2;
        int dist = abs(center - mid);
        if (dist < bestDist) { bestDist = dist; bestIdx = (int)k; }
    }
    int s = blocks[bestIdx].first, e = blocks[bestIdx].second;
    string digits = sandi.substr(s, e - s);

    int ascii_val = 0;
    for (char c : digits) ascii_val = ascii_val*10 + (c - '0');
    if (ascii_val < 0 || ascii_val > 255) { error_msg = "Nilai ASCII tidak valid."; return false; }
    char first_char = (char)ascii_val;

    string rev_cons;
    rev_cons.reserve(n - (e-s));
    for (int idx=0; idx<n; ++idx) {
        if (idx >= s && idx < e) continue; 
        rev_cons.push_back(sandi[idx]);
    }
    string cons = user_defined_reverse(rev_cons); 

    out_first_letter = first_char;
    out_consonant_skeleton.clear();
    for (char c : cons) out_consonant_skeleton.push_back((char)tolower((unsigned char)c));
    return true;
}

string consonant_skeleton_from_word(const string &word) {
    string s;
    for (char c : word) {
        if (!is_vowel(c)) s.push_back((char)tolower((unsigned char)c));
    }
    return s;
}

int main() {
    cout << "Mesin Misterius - Encrypt & Decrypt (parsial)\n";
    cout << "Pilih mode: 1=Encrypt  2=Parse Sandi (dapat cari kandidat dari kamus)\nPilihan: ";
    int mode;
    if (!(cin >> mode)) return 0;
    cin.ignore();

    if (mode == 1) {
        cout << "Masukkan kata asli: ";
        string kata;
        getline(cin, kata);
        string sandi = encrypt_machine(kata);
        cout << "Sandi (hasil mesin): " << sandi << "\n";
        return 0;
    } else if (mode == 2) {
        cout << "Masukkan sandi yang ditemukan: ";
        string sandi;
        getline(cin, sandi);
        char first_letter;
        string cons_skel;
        string err;
        if (!parse_sandi(sandi, first_letter, cons_skel, err)) {
            cout << "Gagal parse sandi: " << err << "\n";
            return 0;
        }
        cout << "Hasil parse:\n";
        cout << "- Kode ASCII -> huruf pertama kata asli: '" << first_letter << "' (ASCII " << (int)(unsigned char)first_letter << ")\n";
        cout << "- Konsonan (skeleton) : \"" << cons_skel << "\" (lowercase)\n\n";

        cout << "Jika ingin mencari kandidat dari file kamus (satu kata per baris), masukkan nama file\n";
        cout << "atau ketik '-' untuk hanya menampilkan skeleton : ";
        string fname;
        getline(cin, fname);
        if (fname == "-") {
            cout << "Selesai. Gunakan skeleton dan huruf pertama untuk menelusuri kemungkinan.\n";
            return 0;
        }

        FILE *f = fopen(fname.c_str(), "r");
        if (!f) {
            cout << "Gagal membuka file kamus: " << fname << "\n";
            cout << "Tetap menampilkan skeleton dan huruf pertama saja.\n";
            return 0;
        }

        cout << "Mencari kandidat di kamus...\n";
        vector<string> matches;
        char linebuf[1024];
        while (fgets(linebuf, sizeof(linebuf), f)) {
            string w = linebuf;
            while (!w.empty() && (w.back()=='\n' || w.back()=='\r')) w.pop_back();
            if (w.empty()) continue;
            if (tolower((unsigned char)w[0]) != tolower((unsigned char)first_letter)) continue;
            string sk = consonant_skeleton_from_word(w);
            if (sk == cons_skel) matches.push_back(w);
        }
        fclose(f);

        if (matches.empty()) {
            cout << "Tidak ditemukan kandidat pada kamus.\n";
        } else {
            cout << "Ditemukan kandidat (" << matches.size() << "):\n";
            for (auto &m : matches) cout << "- " << m << "\n";
        }
        return 0;
    } else {
        cout << "Pilihan mode tidak dikenal.\n";
        return 0;
    }
}
