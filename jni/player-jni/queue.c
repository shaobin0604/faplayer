
#include <malloc.h>
#include <pthread.h>

#include "queue.h"
#include "player.h"

int q_init(queue_t** q) {
    if (*q)
        return -1;
    *q = malloc(sizeof(queue_t));
    if (!(*q))
        return -1;
    (*q)->head = malloc(sizeof(node_t));
    if (!((*q)->head)) {
        free(*q);
        *q = 0;
        return -1;
    }
    (*q)->head->last = (*q)->head;
    (*q)->head->next = (*q)->head;
    (*q)->head->data = 0;
    (*q)->size = 0;
    pthread_mutex_init(&(*q)->mutex, 0);
    pthread_cond_init(&(*q)->condv, 0);
}

int q_push_head(queue_t* q, void* data) {
    node_t* node;
    node_t* head;

    if (!q)
        return -1;
    node = (node_t*) malloc(sizeof(node_t));
    if (!node)
        return -1;
    node->data = data;
    pthread_mutex_lock(&q->mutex);
    q->size++;
    head = q->head;
    node->next = head->next;
    head->next->last = node;
    node->last = head;
    head->next = node;
    pthread_cond_signal(&q->condv);
    pthread_mutex_unlock(&q->mutex);

    return 0;
}

int q_push_tail(queue_t* q, void* data) {
    node_t* node;
    node_t* head;

    if (!q)
        return -1;
    node = (node_t*) malloc(sizeof(node_t));
    if (!node)
        return -1;
    node->data = data;
    pthread_mutex_lock(&q->mutex);
    q->size++;
    head = q->head;
    node->next = head;
    head->last->next = node;
    node->last = head->last;
    head->last = node;
    pthread_cond_signal(&q->condv);
    pthread_mutex_unlock(&q->mutex);

    return 0;
}

void* q_pop_head(queue_t* q) {
    node_t* node;
    node_t* head;
    void* result;

    if (!q)
        return 0;
    pthread_mutex_lock(&q->mutex);
    while (!q->size)
        pthread_cond_wait(&q->condv, &q->mutex);
    if (q->size <= 0)
        result = 0;
    else {
        q->size--;
        head = q->head;
        node = head->next;
        head->next = node->next;
        node->next->last = head;
        result = node->data;
        free(node);
    }
    pthread_mutex_unlock(&q->mutex);

    return result;
}

void* q_pop_tail(queue_t* q) {
    node_t* node;
    node_t* head;
    void* result;

    if (!q)
        return 0;
    pthread_mutex_lock(&q->mutex);
    while (!q->size) {
        pthread_cond_wait(&q->condv, &q->mutex);
    }
    if (q->size <= 0) {
        result = 0;
    }
    else {
        q->size--;
        head = q->head;
        node = head->last;
        head->last = node->last;
        node->last->next = head;
        result = node->data;
        free(node);
    }
    pthread_mutex_unlock(&q->mutex);

    return result;
}

int q_size(queue_t* q) {
    int size;

    if (!q)
        return -1;
    pthread_mutex_lock(&q->mutex);
    size = q->size;
    pthread_mutex_unlock(&q->mutex);

    return size;
}

void q_abort(queue_t* q, void (*func)(void*)) {
   node_t* node;
    void* data;

    if (!q)
        return;
    pthread_mutex_lock(&q->mutex);
    while (q->size > 0) {
        node = q->head->next;
        data = node->data;
        if (data && func)
            func(data);
        q->head->next = node->next;
        free(node);
        q->size--;
    }
    q->head->next = q->head;
    q->head->last = q->head;
    pthread_mutex_unlock(&q->mutex);
}

void q_wake(queue_t* q) {
    if (!q)
        return;
    pthread_mutex_lock(&q->mutex);
    if (!q->size)
        q->size = -1;
    pthread_cond_broadcast(&q->condv);
    pthread_mutex_unlock(&q->mutex);
}

void q_free(queue_t** q, void (*func)(void*)) {
    node_t* node;
    void* data;

    if (!(*q))
        return;
    q_abort(*q, func);
    pthread_mutex_destroy(&(*q)->mutex);
    pthread_cond_destroy(&(*q)->condv);
    free(*q);
    *q = 0;
}

void free_AVPacket(void* data) {
    if (data) {
        av_free_packet((AVPacket*) data);
        av_free((AVPacket*) data);
    }
}

void free_Picture(void* data) {
    if (data) {
        Picture* pic = (Picture*) data;

        avpicture_free(&pic->picture);
        av_free(data);
    }
}

void free_Samples(void* data) {
    if (data) {
        Samples* sam = (Samples*) data;

        av_free(data);
    }
}

IMPLEMENT_QUEUE(audio_packet_queue, AVPacket)
IMPLEMENT_QUEUE(video_packet_queue, AVPacket)
IMPLEMENT_QUEUE(subtitle_packet_queue, AVPacket)
IMPLEMENT_QUEUE(picture_queue, Picture)
IMPLEMENT_QUEUE(samples_queue, Samples)

int queue_init() {
    int err;

    if (audio_packet_queue || video_packet_queue || subtitle_packet_queue || picture_queue || samples_queue)
        return -1;
    if (gCtx->audio_enabled) {
        err = audio_packet_queue_init();
        if (err < 0)
            goto fail;
        err = samples_queue_init();
        if (err < 0)
            goto fail;
    }
    if (gCtx->video_enabled) {
        err = video_packet_queue_init();
        if (err < 0)
            goto fail;
        err = picture_queue_init();
        if (err < 0)
            goto fail;
    }
    if (gCtx->subtitle_enabled) {
        err = subtitle_packet_queue_init();
        if (err < 0)
            goto fail;
    }
    return 0;
fail:
    queue_free();
    return -1;
}

void queue_free() {
    if (audio_packet_queue)
        audio_packet_queue_free();
    if (video_packet_queue)
        video_packet_queue_free();
    if (subtitle_packet_queue)
        subtitle_packet_queue_free();
    if (picture_queue)
        picture_queue_free();
    if (samples_queue)
        samples_queue_free();
}

