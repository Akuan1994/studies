/* associative array */

/* ��ʼ��hash�� */
void assoc_init(const int hashpower_init);

/* ��hash���в���key��Ӧ��item */
item *assoc_find(const char *key, const size_t nkey, const uint32_t hv);

/* ��hash���в���һ��item���������Ͱ����1.5����������չ  */
int assoc_insert(item *item, const uint32_t hv);

/* ��hash����ɾ��ָ��keyֵ��Ӧ��item */
void assoc_delete(const char *key, const size_t nkey, const uint32_t hv);

void do_assoc_move_next_bucket(void);

/* ����hash����չ�߳� */
int start_assoc_maintenance_thread(void);

/* ֹͣhash����չ�߳� */
void stop_assoc_maintenance_thread(void);

/* hashͰ������2��ָ��ֵ */
extern unsigned int hashpower;
