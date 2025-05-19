#pragma once
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

dev_t get_file_device(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return st.st_dev;  // Возвращает ID устройства
}

std::string find_mount_device(dev_t dev) {
    FILE *fp = fopen("/proc/self/mountinfo", "r");
    if (!fp) {
        perror("fopen failed");
        return std::string();
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        unsigned int maj, min;
        char device[256];
        if (sscanf(line, "%*d %*d %u:%u %*s %*s %*s %*s %*s %*s %s", &maj, &min, device) == 3) {
            if (maj == major(dev) && min == minor(dev)) {
                fclose(fp);
                return std::string(device);  // Нашли устройство!
            }
        }
    }

    fclose(fp);
    return std::string();  // Не найдено
}

bool is_at_hdd(int fd){
    bool is_rotational = (ioctl(fd, BLKROTATIONAL, &rot) == 0 && rot);
}
    //TODO for SSD/HDD
    //bool is_rotational = (ioctl(fd, BLKROTATIONAL, &rot) == 0 && rot;
    //__builtin_prefetch(data + offset);
    //#pragma omp parallel for schedule(dynamic, 256*1024)
    //posix_memalign(&buffer, 4096, 1 << 20);