#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MAX_PEOPLE 10
#define POISSON_MEAN 3

// Struktura reprezentujaca osobe w windzie
typedef struct {
    int destination_floor;  // Docelowe pietro osoby
} Person;

// Funkcja generujaca liczbe osob zgodnie z rozkladem Poissona
int poisson_distribution(double lambda) {
    double L = exp(-lambda);  // Wyliczenie parametru L dla rozkladu Poissona
    int k = 0;  // Liczba wygenerowanych zdarzen
    double p = 1.0;  // Prawdopodobienstwo
    do {
        k++;  // Zwiekszenie liczby zdarzen
        p *= (double)rand() / RAND_MAX;  // Aktualizacja prawdopodobienstwa
    } while (p > L);  // Warunek zakonczenia
    return k - 1;  // Zwraca liczbe osob
}

// Funkcja sprawdzajaca, czy sa jeszcze osoby czekajace na winde
int are_people_waiting(int* people_waiting, int floors) {
    for (int i = 0; i < floors; i++) {  // Iteracja po wszystkich pietrach
        if (people_waiting[i] > 0) {  // Sprawdzenie, czy ktos czeka na winde
            return 1;  // Zwrocenie wartosci true, jesli ktos czeka
        }
    }
    return 0;  // Zwrocenie wartosci false, jesli nikt nie czeka
}

// Funkcja symulujaca przejazd windy
void elevator_simulation(int floors, int capacity) {
    int elevator_position = 0;  // Poczatkowa pozycja windy
    int people_in_elevator = 0;  // Liczba osob w windzie
    int* people_waiting = (int*)malloc(floors * sizeof(int));  // Tablica liczby osob czekajacych na kazdym pietrze
    Person* elevator = (Person*)malloc(capacity * sizeof(Person));  // Tablica osob w windzie
    int direction = 1;  // 1 dla jazdy w gore, -1 dla jazdy w dol

    // Inicjalizacja liczby osob czekajacych na kazdym pietrze
    for (int i = 0; i < floors; i++) {
        people_waiting[i] = poisson_distribution(POISSON_MEAN);
    }

    while (people_in_elevator > 0 || are_people_waiting(people_waiting, floors)) {
        // Wyswietlenie stanu windy i budynku
#ifdef _WIN32
        system("cls");  // Czyszczenie ekranu w Windows
#else
        system("clear");  // Czyszczenie ekranu w Unix/Linux
#endif

        for (int i = floors - 1; i >= 0; i--) {  // Iteracja po wszystkich pietrach
            if (i == elevator_position) {  // Sprawdzenie pozycji windy
                printf("[ E | %2d ]\t People waiting: %d\n", people_in_elevator, people_waiting[i]);  // Winda na pietrze
            }
            else {
                printf("[   |    ]\t People waiting: %d\n", people_waiting[i]);  // Winda nie jest na pietrze
            }
        }
        printf("\n");

        // Symulowanie wyjscia ludzi z windy na odpowiednich pietrach
        for (int i = 0; i < people_in_elevator; i++) {
            if (elevator[i].destination_floor == elevator_position) {  // Sprawdzenie, czy osoba chce wysiasc na tym pietrze
                // Przesuniecie osob w windzie, aby zapelnic wolne miejsce
                for (int j = i; j < people_in_elevator - 1; j++) {
                    elevator[j] = elevator[j + 1];
                }
                people_in_elevator--;  // Zmniejszenie liczby osob w windzie
                i--;  // Skorygowanie indeksu po przesunieciu
            }
        }

        // Symulowanie wejscia ludzi do windy
        if (people_waiting[elevator_position] > 0 && people_in_elevator < capacity) {
            int boarding = capacity - people_in_elevator;  // Liczba osob, ktore moga wejsc do windy
            if (boarding > people_waiting[elevator_position]) {
                boarding = people_waiting[elevator_position];  // Korekcja liczby wchodzacych osob
            }
            for (int i = 0; i < boarding; i++) {
                elevator[people_in_elevator].destination_floor = rand() % floors;  // Losowanie docelowego pietra
                people_in_elevator++;  // Zwiekszenie liczby osob w windzie
            }
            people_waiting[elevator_position] -= boarding;  // Zmniejszenie liczby osob czekajacych
        }

        // Optymalizacja ruchu windy: najblizsze pietro
        int next_position = -1;
        int min_distance = floors;
        for (int i = 0; i < people_in_elevator; i++) {
            int distance = abs(elevator[i].destination_floor - elevator_position);  // Obliczenie dystansu
            if (distance < min_distance) {
                min_distance = distance;
                next_position = elevator[i].destination_floor;  // Aktualizacja najblizszego pietra
            }
        }
        if (next_position == -1) {  // Jesli nie ma pasazerow, jedz do najblizszego pietra z oczekujacymi ludzmi
            for (int i = 0; i < floors; i++) {
                if (people_waiting[i] > 0) {
                    int distance = abs(i - elevator_position);  // Obliczenie dystansu
                    if (distance < min_distance) {
                        min_distance = distance;
                        next_position = i;  // Aktualizacja najblizszego pietra z oczekujacymi ludzmi
                    }
                }
            }
        }

        // Aktualizacja pozycji windy
        if (next_position != -1) {
            if (next_position > elevator_position) {
                elevator_position++;  // Przemieszczenie windy w gore
            }
            else if (next_position < elevator_position) {
                elevator_position--;  // Przemieszczenie windy w dol
            }
        }

        // Spowolnienie symulacji
#ifdef _WIN32
        Sleep(1000);  // Sleep w milisekundach w Windows
#else
        sleep(1);  // Sleep w sekundach w Unix/Linux
#endif
    }

    free(people_waiting);  // Zwolnienie pamieci
    free(elevator);  // Zwolnienie pamieci
}

int main() {
    srand(time(NULL));  // Inicjalizacja generatora liczb pseudolosowych

    int floors, capacity;

    printf("Podaj liczbe pieter w budynku: ");
    scanf_s("%d", &floors);
    printf("Podaj pojemnosc windy: ");
    scanf_s("%d", &capacity);

    if (floors <= 0 || capacity <= 0) {
        printf("Liczba pieter i pojemnosc windy musza byc wieksze niz 0.\n");
        return 1;
    }

    elevator_simulation(floors, capacity);  // Uruchomienie symulacji windy

    return 0;
}
