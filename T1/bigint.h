typedef unsigned char *BigInt;

/* nbits sempre maior que 0 e múltiplo de 8 */
void bi_init (int nbits);

/* cria novo BigInt com valor inicial val */
BigInt bi_new (int val);

/* destroi BigInt */
void bi_destroy (BigInt a);

/* a + b */
BigInt bi_sum (BigInt a, BigInt b);

/* -a */
BigInt bi_minus (BigInt a);

/* a - b */
BigInt bi_sub (BigInt a, BigInt b);

/* a * b */
BigInt bi_mul (BigInt a, BigInt b);

/* desloca a de n bits para a direita */
BigInt bi_shr (BigInt a, int n);

/* desloca a de n bits para a esquerda */
BigInt bi_shl (BigInt a, int n);