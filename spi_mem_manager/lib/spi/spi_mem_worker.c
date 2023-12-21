#include "spi_mem_worker_i.h"

typedef enum {
    SPIMemEventStopThread = (1 << 0),
    SPIMemEventChipDetect = (1 << 1),
    SPIMemEventRead = (1 << 2),
    SPIMemEventVerify = (1 << 3),
    SPIMemEventErase = (1 << 4),
    SPIMemEventWrite = (1 << 5),
    SPIMemEventAll =
        (SPIMemEventStopThread | SPIMemEventChipDetect | SPIMemEventRead | SPIMemEventVerify |
         SPIMemEventErase | SPIMemEventWrite)
} SPIMemEventEventType;

static int32_t spi_mem_worker_thread(void* thread_context);

SPIMemWorker* spi_mem_worker_alloc() {
    FURI_LOG_T("myCustomApp", "W3Hello World: %s", "This is an example logger"); 
    SPIMemWorker* worker = malloc(sizeof(SPIMemWorker));
    FURI_LOG_T("myCustomApp", "W4Hello World: %s", "This is an example logger"); 
    worker->callback = NULL;
    FURI_LOG_T("myCustomApp", "W5Hello World: %s", "This is an example logger"); 
    worker->thread = furi_thread_alloc();
    FURI_LOG_T("myCustomApp", "W6Hello World: %s", "This is an example logger"); 
    worker->mode_index = SPIMemWorkerModeIdle;
    FURI_LOG_T("myCustomApp", "W7Hello World: %s", "This is an example logger"); 
    furi_thread_set_name(worker->thread, "SPIMemWorker");
    FURI_LOG_T("myCustomApp", "W8Hello World: %s", "This is an example logger"); 
    furi_thread_set_callback(worker->thread, spi_mem_worker_thread);
    FURI_LOG_T("myCustomApp", "W9Hello World: %s", "This is an example logger"); 
    furi_thread_set_context(worker->thread, worker);
    FURI_LOG_T("myCustomApp", "W10Hello World: %s", "This is an example logger"); 
    furi_thread_set_stack_size(worker->thread, 10240);
    FURI_LOG_T("myCustomApp", "W11Hello World: %s", "This is an example logger"); 
    return worker;
}

void spi_mem_worker_free(SPIMemWorker* worker) {
    furi_thread_free(worker->thread);
    free(worker);
}

bool spi_mem_worker_check_for_stop(SPIMemWorker* worker) {
    UNUSED(worker);
    uint32_t flags = furi_thread_flags_get();
    return (flags & SPIMemEventStopThread);
}

static int32_t spi_mem_worker_thread(void* thread_context) {
    FURI_LOG_E("myCustomApp", "Hello World: %s", "This is an example logger"); 
    SPIMemWorker* worker = thread_context;
    while(true) {
        uint32_t flags = furi_thread_flags_wait(SPIMemEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(flags != (unsigned)FuriFlagErrorTimeout) {
            if(flags & SPIMemEventStopThread) break;
            //if(flags & SPIMemEventChipDetect) worker->mode_index = SPIMemWorkerModeChipDetect;
            if(flags & SPIMemEventRead) worker->mode_index = SPIMemWorkerModeRead;
            if(flags & SPIMemEventVerify) worker->mode_index = SPIMemWorkerModeVerify;
            if(flags & SPIMemEventErase) worker->mode_index = SPIMemWorkerModeErase;
            if(flags & SPIMemEventWrite) worker->mode_index = SPIMemWorkerModeWrite;
            if(spi_mem_worker_modes[worker->mode_index].process) {
                spi_mem_worker_modes[worker->mode_index].process(worker);
            }
            worker->mode_index = SPIMemWorkerModeIdle;
        }
    }
    return 0;
}

void spi_mem_worker_start_thread(SPIMemWorker* worker) {
    furi_thread_start(worker->thread);
}

void spi_mem_worker_stop_thread(SPIMemWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventStopThread);
    furi_thread_join(worker->thread);
}

void spi_mem_worker_chip_detect_start(
    SPIMemChip* chip_info,
    found_chips_t* found_chips,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    void* context) {
    furi_check(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    worker->found_chips = found_chips;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventChipDetect);
}

void spi_mem_worker_read_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    void* context) {
    furi_check(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventRead);
}

void spi_mem_worker_verify_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    void* context) {
    furi_check(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventVerify);
}

void spi_mem_worker_erase_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    void* context) {
    furi_check(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventErase);
}

void spi_mem_worker_write_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    void* context) {
    furi_check(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventWrite);
}
