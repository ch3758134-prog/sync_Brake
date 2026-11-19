#ifndef BRAKESYNC_EVB_APP_H
#define BRAKESYNC_EVB_APP_H

typedef enum {
    APP_STATUS_OK = 0,
    APP_STATUS_ERROR
} app_status_t;

app_status_t app_init(void);
app_status_t app_run_once(void);

#endif
