//
// Created by Monokai on 2022/8/4.
//

#include <event2/event.h>

int main() {
    // 列出支持的方法
    int i;
    const char **methods = event_get_supported_methods();
    printf("Starting Libevent %s.  Available methods are:\n", event_get_version());
    for (i = 0; methods[i] != NULL; ++i) {
        printf("%s\n", methods[i]);
    }
    printf("#################\n");

    /*
     * event_base_get_method()返回 event_base 正在使用的方法。
     * event_base_get_features ()返回 event_base 支持的特征的比特掩码。
     * */
    struct event_base *base;
    enum event_method_feature f;

    base = event_base_new();
    if (!base) {
        puts("Couldn't get an event_base!\n");
    } else {
        printf("Using Libevent with backend method %s.\n",
               event_base_get_method(base));
        f = event_base_get_features(base);
        if ((f & EV_FEATURE_ET))
            printf("  Edge-triggered events are supported.\n");
        if ((f & EV_FEATURE_O1))
            printf("  O(1) event notification is supported.\n");
        if ((f & EV_FEATURE_FDS))
            printf("  All FD types are supported.\n");
        puts("");
    }


}
