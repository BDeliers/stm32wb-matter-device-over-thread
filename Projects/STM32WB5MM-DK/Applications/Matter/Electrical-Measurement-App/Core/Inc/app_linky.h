#ifndef APP_LINKY_H
#define APP_LINKY_H

#include <stdint.h>
#include <stddef.h>

/*
 * The Linky incoming bytes will be processed and parsed in this module
 */
class AppLinky 
{
public:
    static AppLinky& GetInstance(void)
    {
        static AppLinky singleton;
        return singleton;
    }

    bool Init(void);
    bool StartReceiving(void);

    static void UartRxCallback(uint8_t byte);

    enum Field : uint8_t {
        ADSC,           // Adresse Secondaire du Compteur
        VTIC,           // Version de la TIC
        DATE,           // Date et heure courante
        NGTF,           // Nom du calendrier tarifaire fournisseur
        LTARF,          // Libellé tarif fournisseur en cours
        EAST,           // Energie active soutirée totale (Wh)
        EASF01,         // Energie active soutirée Fournisseur, index 01 (Wh)
        EASF02,         // Energie active soutirée Fournisseur, index 02 (Wh)
        EASF03,         // Energie active soutirée Fournisseur, index 03 (Wh)
        EASF04,         // Energie active soutirée Fournisseur, index 04 (Wh)
        EASF05,         // Energie active soutirée Fournisseur, index 05 (Wh)
        EASF06,         // Energie active soutirée Fournisseur, index 06 (Wh)
        EASF07,         // Energie active soutirée Fournisseur, index 07 (Wh)
        EASF08,         // Energie active soutirée Fournisseur, index 08 (Wh)
        EASF09,         // Energie active soutirée Fournisseur, index 09 (Wh)
        EASF10,         // Energie active soutirée Fournisseur, index 10 (Wh)
        EASD01,         // Energie active soutirée Distributeur, index 01 (Wh)
        EASD02,         // Energie active soutirée Distributeur, index 02 (Wh)
        EASD03,         // Energie active soutirée Distributeur, index 03 (Wh)
        EASD04,         // Energie active soutirée Distributeur, index 04 (Wh)
        EAIT,           // Energie active injectée totale (Wh)
        ERQ1,           // Energie réactive Q1 totale (VArh)
        ERQ2,           // Energie réactive Q2 totale (VArh)
        ERQ3,           // Energie réactive Q3 totale (VArh)
        ERQ4,           // Energie réactive Q4 totale (VArh)
        IRMS1,          // Courant efficace, phase 1 (A)
        IRMS2,          // Courant efficace, phase 2 (A)
        IRMS3,          // Courant efficace, phase 3 (A)
        URMS1,          // Tension efficace, phase 1 (V)
        URMS2,          // Tension efficace, phase 2 (V)
        URMS3,          // Tension efficace, phase 3 (V)
        PREF,           // Puissance app. de référence (PREF (kVA))
        PCOUP,          // Puissance app. de coupure (PCOUP (kVA))
        SINSTS,         // Puissance app. Instantanée soutirée (VA)
        SINSTS1,        // Puissance app. Instantanée soutirée phase 1 (VA)
        SINSTS2,        // Puissance app. instantanée soutirée phase 2 (VA)
        SINSTS3,        // Puissance app. instantanée soutirée phase 3 (VA)
        SMAXSN,         // Puissance app. max. soutirée n (VA)
        SMAXSN1,        // Puissance app. max. soutirée n phase 1 (VA)
        SMAXSN2,        // Puissance app. max. soutirée n phase 2 (VA)
        SMAXSN3,        // Puissance app. max. soutirée n phase 3 (VA)
        SMAXSN_M1,      // Puissance app max. soutirée n-1 (VA)
        SMAXSN1_M1,     // Puissance app max. soutirée n-1 phase 1 (VA)
        SMAXSN2_M1,     // Puissance app max. soutirée n-1 phase 2 (VA)
        SMAXSN3_M1,     // Puissance app max. soutirée n-1 phase 3 (VA)
        SINSCTI,        // Puissance app. Instantanée injectée (VA)
        SMAXIN,         // Puissance app. max. injectée n (VA)
        SMAXIN_M1,      // Puissance app max. injectée n-1 (VA)
        CCASN,          // Point n de la courbe de charge active soutirée (W)
        CCASN_M1,       // Point n-1 de la courbe de charge active soutirée (W)
        CCAIN,          // Point n de la courbe de charge active injectée (W)
        CCAIN_M1,       // Point n-1 de la courbe de charge active injectée (W)
        UMOY1,          // Tension moy. ph. 1 (V)
        UMOY2,          // Tension moy. ph. 2 (V)
        UMOY3,          // Tension moy. ph. 3 (V)
        STGE,           // Registre de Statuts
        DPM1,           // Début Pointe Mobile 1
        FPM1,           // Fin Pointe Mobile 1
        DPM2,           // Début Pointe Mobile 2
        FPM2,           // Fin Pointe Mobile 2
        DPM3,           // Début Pointe Mobile 3
        FPM3,           // Fin Pointe Mobile 3
        MSG1,           // Message court
        MSG2,           // Message Ultra court
        PRM,            // PRM
        RELAIS,         // Relais
        NTARF,          // Numéro de l’index tarifaire en cours
        NJOURF,         // Numéro du jour en cours calendrier fournisseur
        NJOURF_P1,      // Numéro du prochain jour calendrier fournisseur
        PJOURF_P1,      // Profil du prochain jour calendrier fournisseur
        PPOINTE,        // Profil du prochain jour de pointe
        Field_COUNT,
    };

    enum Season : uint8_t {
        WINTER,
        SPRING,
        SUMMER,
        AUTUMN,
    };

private:
    uint8_t buffer_incoming_bytes[1256];
    uint8_t* ptr_incoming_bytes;
    bool frame_started;
    bool frame_received;
    bool ready_to_receive;

    static void Handler(void* arg);

    bool CheckCrc(const char* data, char checksum, size_t size);
    bool ProcessLine(char* start, char* end);
    bool ParseAndSaveDate(const char* data);
    Field StringToField(const char* str);

    // Linky data
    Season   season;
    uint16_t year, month, day;
    uint16_t hour, minute, second;

    uint32_t linky_data[Field::Field_COUNT];
    char     linky_address[13];
    char     linky_calname[17];
    char     linky_curprice[17];
    char     linky_prm[15];

};

#endif // APP_LINKY_H