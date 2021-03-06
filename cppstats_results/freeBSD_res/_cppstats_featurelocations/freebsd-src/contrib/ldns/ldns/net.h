











#if !defined(LDNS_NET_H)
#define LDNS_NET_H

#include <ldns/ldns.h>
#include <sys/socket.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LDNS_DEFAULT_TIMEOUT_SEC 5
#define LDNS_DEFAULT_TIMEOUT_USEC 0

















ldns_status ldns_udp_send(uint8_t **result, ldns_buffer *qbin, const struct sockaddr_storage *to, socklen_t tolen, struct timeval timeout, size_t *answersize);










int ldns_udp_bgsend(ldns_buffer *qbin, const struct sockaddr_storage *to, socklen_t tolen, struct timeval timeout);










int ldns_tcp_bgsend(ldns_buffer *qbin, const struct sockaddr_storage *to, socklen_t tolen, struct timeval timeout);












ldns_status ldns_tcp_send(uint8_t **result, ldns_buffer *qbin, const struct sockaddr_storage *to, socklen_t tolen, struct timeval timeout, size_t *answersize);










ldns_status ldns_send(ldns_pkt **pkt, ldns_resolver *r, const ldns_pkt *query_pkt);











ldns_status ldns_send_buffer(ldns_pkt **pkt, ldns_resolver *r, ldns_buffer *qb, ldns_rdf *tsig_mac);








int ldns_tcp_connect(const struct sockaddr_storage *to, socklen_t tolen, struct timeval timeout);







int ldns_udp_connect(const struct sockaddr_storage *to, struct timeval timeout);










ssize_t ldns_tcp_send_query(ldns_buffer *qbin, int sockfd, const struct sockaddr_storage *to, socklen_t tolen);










ssize_t ldns_udp_send_query(ldns_buffer *qbin, int sockfd, const struct sockaddr_storage *to, socklen_t tolen);










uint8_t *ldns_tcp_read_wire_timeout(int sockfd, size_t *size, struct timeval timeout);










uint8_t *ldns_tcp_read_wire(int sockfd, size_t *size);











uint8_t *ldns_udp_read_wire(int sockfd, size_t *size, struct sockaddr_storage *fr, socklen_t *frlen);









struct sockaddr_storage * ldns_rdf2native_sockaddr_storage(const ldns_rdf *rd, uint16_t port, size_t *size);







ldns_rdf * ldns_sockaddr_storage2rdf(const struct sockaddr_storage *sock, uint16_t *port);









ldns_status ldns_axfr_start(ldns_resolver *resolver, const ldns_rdf *domain, ldns_rr_class c);

#if defined(__cplusplus)
}
#endif

#endif
