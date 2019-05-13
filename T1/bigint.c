/* Lucas Eduardo P. Martins  1013475 */

#include<stdio.h>
#include<stdlib.h>
#include "bigint.h"

int numero_bits;


/* nbits sempre maior que 0 e múltiplo de 8 */
void bi_init (int nbits)
{
  numero_bits = nbits;
}


/* cria novo BigInt com valor inicial val */
BigInt bi_new (int val)
{
  int npos = numero_bits/8;
  int temp = val,i,compl;
  BigInt novo = (unsigned char*)malloc(npos*(sizeof(unsigned char)));

  for(i=0;i < npos;i++)
	  novo[i] = 0;
   
  for(i=0;i<4;i++) /* colocando 8 bits do int em cada posição */
  {
    if(i==0)
      temp<<=24;
    else
      if(i==1)
       temp<<=16;
      else
        if(i==2)
          temp<<=8;
    temp>>=24;	
    novo[i] = temp;
	temp = val;   
  }  
  
  compl = (numero_bits - 32)/8;  /* completar os bits vagos */
  if(compl!=0)
  {
    temp = val;
    temp>>=31;
    if(temp==0)
    {
      for(i=4;i < npos;i++)
         novo[i] = 0;
    }
    else
    {
      for(i=4;i < npos;i++)       /* número negativo */
         novo[i] = 0xFF;
    }
   }
 
return novo;
}


/* destroi BigInt */
void bi_destroy (BigInt a)
{
  int pos = 0,npos = numero_bits/8;

  while(pos < npos)
  {
    a[pos] = 0;
    pos++;
  }
  free(a);
}

/* função auxiliar */
static int vai_um(unsigned char a,unsigned char b)
{
  unsigned short s;

  s = a+b;
  s>>=8;
  if(s==1)
     return 1;                /* overflow */
  return 0;              /* sem overflow */                     
}

/* a + b */
BigInt bi_sum (BigInt a, BigInt b)
{
  short i,npos = numero_bits/8,aux=0;  /* se aux for 1,indicará overflow na posição anterior */
  int temp,temp2;          /* declaradas para guardar o retorno da função vai_um */
  unsigned char z = 0x01;
  BigInt res = bi_new(0);
  
  for(i=0;i < npos;i++)
  {
    temp = vai_um(a[i],b[i]);

    if((temp == 0 && aux == 0))
      res[i] = a[i] + b[i];
    else
      if(temp == 0 && aux == 1)
      {
        res[i] = a[i] + b[i];
        temp2 = vai_um(res[i],z);

        if(temp2 == 0)         /* verifica se somar 1 acarreta overflow */
        {
          res[i]+=1;
          aux = 0;
        }
        else
          if(temp2 == 1)
            res[i]+=1;
      }
      else
        if(temp == 1 && aux == 0)
        {
          res[i] = a[i] + b[i];
          aux = 1;
        }
        else
          if(temp == 1 && aux == 1)
          {
            res[i] = a[i] + b[i];
            res[i]+=1;
          }
  }
 return res;    
}

static int verifica_zero(BigInt a)
{
	int i,npos = numero_bits/8;
	for(i=0;i<npos;i++)
	{
		if(a[i]!=0)
			return 1;
	}
	return 0;
}


/* -a */
BigInt bi_minus (BigInt a)
{
  int pos = 0,npos = numero_bits/8,temp,ret = 0;
  unsigned char c = 0x01;

  ret = verifica_zero(a);
  if(ret == 0)
	  return a;

  while(pos < npos)
  {
    a[pos] = ~a[pos];
    pos++;
  }
  temp = vai_um(a[0], c);
  if(temp == 0)
      a[0]+= 1;        /* sem problema de overflow */
  else
  {                /* achar primeira posição onde não ocorre overflow */
     pos = 1;
     while(pos < npos)
     {
       a[pos-1]+= 1;
       if(vai_um(a[pos], c)==1)
         pos++;
       else
       {
         a[pos]+= 1;
         break;
       }
     }
  }
  return a;
}

/* a - b */
BigInt bi_sub (BigInt a, BigInt b)
{
  int npos = numero_bits/8,i;  
  BigInt resp,ret,aux = bi_new(0);
  
  for(i=0;i < npos;i++)
      aux[i] = b[i];
    
  ret = bi_minus(aux);
  resp = bi_sum(a,ret);
    
  return resp;
}

/* função auxiliar */
static int verifica_bit(unsigned char a,int n)     /* n é o número do bit investigado  */
{
	int e = 8-n,d = 7;
	unsigned char aux = a;

	aux<<=e;
	aux>>=d;

	if(aux==1)
		return 1;

	return 0;
}

/* a * b */
BigInt bi_mul (BigInt a, BigInt b)
{
  int npos = numero_bits/8,i,temp=0,n,j,cont=0;  
  BigInt aux1 = bi_new(0),aux2 = bi_new(0),aux3,resp,velho;

  for(i=0;i < npos;i++)
  {
    aux1[i] = a[i];	
  }

  for(i=0;i < npos;i++)
  {
	  for(j=0,n=1;j<8;j++,n++)
	  {
		temp = verifica_bit(b[i],n);
		if(temp==0)
		{
			if(cont==0)
				resp = aux2;
			cont++;
		}
		else
			if(temp==1)
			{
				if(cont==0)
				{
					resp = bi_sum(aux1,aux2);
					cont++;
				}
				else
				{
					velho = resp;
					aux3 = bi_shl(aux1,cont);
					resp = bi_sum(velho,aux3);
					cont++;
				}
			}
	  }
  }
  free(aux1);
  free(aux2);
  return resp;
}


/* desloca a de n bits para a direita */
BigInt bi_shr (BigInt a, int n)
{
  int pos,npos = numero_bits/8,i,menos;
  int d = n/8,res = n%8;
  unsigned char temp,temp2,temp3;
  BigInt aux = bi_new(0);

  if(n > numero_bits)
  {
	  printf("Numero de bits excedido\n");
	  exit(1);
  }

  for(i=0;i < npos;i++)
    aux[i] = a[i];

  temp3 = aux[npos-1]>>7;    /* descobrir se faz shift aritmético ou lógico */
    
  if(temp3==0)  /* shift lógico */
  {
    
    if((d == 0)||(d == 1 && res == 0))        /* menor ou igual a 8 */
    {
      menos = 8-n;
      for(i=npos-1;i > -1;i--)
      {
        if(i==npos-1)
        {
          temp2 = aux[i];
          aux[i]>>=n;
          temp2<<=menos;
        }
        else
        {
          temp = aux[i];
          aux[i]>>=n;
          aux[i]|=temp2;
          temp2 = temp<<menos;
        }
      }    
    }
	else
		if(d > 1 && res == 0)              /* múltiplos de 8 */
		{
			  for(pos = 0;i < d;pos++)				  aux[pos] = 0;			  for(pos = 0,i=pos+d;i < npos;pos++,i++)				  aux[pos] = a[i];
			  while(pos < npos)
			  {				  aux[pos] = 0;				  pos++;
			  }	  
		}
		else
			if(d > 0 && res > 0)     /* maiores que 8*/
			{
				  for(pos = 0;i < d;pos++)				  aux[pos] = 0;			      for(pos = 0,i=pos+d;i < npos;pos++,i++)				      aux[pos] = a[i];
				  while(pos <  npos)
				  {					aux[pos] = 0;					pos++;
				  }	  
				  menos = 8 - res;
				  for(i=npos-1;i > -1;i--)
				  {
					 if(i==npos-1)
					 {
						temp2 = aux[i];
						aux[i]>>=res;
						temp2<<=menos;
					 }
					else
                    {
						temp = aux[i];
						aux[i]>>=res;
						aux[i]|=temp2;
						temp2 = temp<<menos;
					}
				  }    
			}
  }
  else
	  if(temp3==1) /*shift aritmético*/ 
  {
    
    if((d == 0)||(d == 1 && res == 0))   /* menor ou igual a 8*/
    {
      menos = 8-n;
      for(i=npos-1;i > -1;i--)
      {
        if(i==npos-1)
        {
          temp2 = aux[i];
          aux[i]>>=n;
          temp2<<=menos;
          for(pos=0;pos < n;pos++)   /* completando os bits com 1 */ 
          {
            if(pos == 0)
            {
              aux[i]|=0x80;
              temp = 0x80;
            }
            else
            {
              temp>>=1;
              aux[i]|=temp;
            }
          }
        }
        else
        {
          temp = aux[i];
          aux[i]>>=n;
          aux[i]|=temp2;
          temp2 = temp<<menos;
        }
       }
     }
	else
		if(d > 1 && res == 0)              /* múltiplos de 8 */
		{			for(pos = 0;i < d;pos++)				  aux[pos] = 0;			for(pos = 0,i=pos+d;i < npos;pos++,i++)				  aux[pos] = a[i];			
			while(pos < npos)			{				aux[pos] = 0xFF;				pos++;			}	  
	     
		}
		else
			if(d > 0 && res > 0)     /* maiores que 8*/
			{
				for(pos = 0;i < d;pos++)				  aux[pos] = 0;				for(pos = 0,i=pos+d;i < npos;pos++,i++)				  aux[pos] = a[i];			
				while(pos < npos)				{					aux[pos] = 0xFF;					pos++;				}	     

				 menos = 8-res;
				for(i=npos-1;i > -1;i--)
				{
					if(i==npos-1)
					{
						temp2 = aux[i];
						aux[i]>>=res;
						temp2<<=menos;
						for(pos=0;pos < n;pos++)   /* completando os bits com 1 */ 
						{
							if(pos == 0)
							{
								aux[i]|=0x80;
								temp = 0x80;
							}
							else
							{
								temp>>=1;
								aux[i]|=temp;
							}
						}
					}
					else
					{
						temp = aux[i];
						aux[i]>>=res;
						aux[i]|=temp2;
						temp2 = temp<<menos;
					}
				}
			}
	  }
  return aux;
   
}

/* desloca a de n bits para a esquerda */
BigInt bi_shl (BigInt a, int n)
{
  int pos,npos = numero_bits/8,i,menos;
  int d = n/8,res = n%8;
  unsigned char temp,temp2;
  BigInt aux = bi_new(0);

  if(n > numero_bits)
  {
	  printf("Numero de bits excedido\n");
	  exit(1);
  }

  for(i=0;i < npos;i++)
    aux[i] = a[i];
  
  if(d > 1 && res == 0)   /* múltiplos de 8 */
  {
	  for(pos = npos-1,i=0;i < d;pos--,i++)
	  	  aux[pos] = 0;
	  for(pos = npos-1,i=pos-d;i > -1;pos--,i--)
		  aux[pos] = a[i];
	  while(pos > -1)
	  {
		  aux[pos] = 0;
		  pos--;
	  }
	  
  }
  else
	  if(d > 0 && res > 0)  /* maiores que 8 */
	  {
		  for(pos = npos-1,i=0;i < d;pos--,i++)
	  	     aux[pos] = 0;
	      for(pos = npos-1,i=pos-d;i > -1;pos--,i--)
		     aux[pos] = a[i];
	      while(pos > -1)
	      {
		    aux[pos] = 0;
		    pos--;
	      }

		  menos = 8 - res;
          for(i=0;i < npos;i++)
          {
            if(i==0)
            {
              temp2 = aux[i];
              aux[i]<<=res;
              temp2>>=menos;
            }
            else
            {
              temp = aux[i];
              aux[i]<<=res;
              aux[i]|=temp2;
              temp2 = temp>>menos;
            }
          }    
	  }
      else
        if((d == 0)||(d == 1 && res == 0))  // menor ou igual a 8
        {
          menos = 8-n;
          for(i=0;i < npos;i++)
          {
            if(i==0)
            {
              temp2 = aux[i];
              aux[i]<<=n;
              temp2>>=menos;
            }
            else
            {
              temp = aux[i];
              aux[i]<<=n;
              aux[i]|=temp2;
              temp2 = temp>>menos;
            }
          }    
        }
  return aux;
}
