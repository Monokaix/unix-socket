# Introduce

Some unix socket programming demos including epoll and libevent.

# epoll-demo

## make

```shell
$ cd epoll-demo && mkdir build && cd build
$ cmake ..
$ make
```

## usage

```shell
$ server: ./epoll_demo_server
$ client: ./epoll_demo_client
```

# libevent-demo

## make

```shell
$ cd libevent-demo && mkdir build && cd build
$ cmake ..
$ make
```

## usage

```shell
$ server: ./libevent_demo
$ client: curl 127.0.0.1:8080
```

