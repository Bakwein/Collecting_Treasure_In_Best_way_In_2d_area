#include "PROLAB.h"

//derlemek için
//gcc -Wall -Wextra -Werror -lcurl main.c

char *return_value; // global
int  return_value_size; // global

//kontrol için
void print_array(char **arr)
{
    for(int i = 0;arr[i] != NULL;i++)
    {
        printf("%s\n",arr[i]);
    }
}

void print_array2(int *arr)
{
    for(int i = 0;arr[i] != -1;i++)
    {
        printf("%d\n",arr[i]);
    }
}


size_t WriteMemoryCallback(char *buf, size_t size, size_t num_item, void *s)
{
    //for flags
    (void)s;
    //printf("size : %zu\n",size); 1
    //printf("num_item : %zu\n",num_item); 99

    size_t bytes = size * num_item;
    return_value = (char*)malloc(bytes);
    int newline_flag = 0;
    for(size_t i = 0;i<bytes;i++) // using size_t cuz -w... flag
    {
        if(buf[i] == '\n')
        {
            return_value_size++;
            newline_flag = 1;
        }
        return_value[i] = buf[i];
    }
    if(newline_flag == 1)
        return_value_size++;
    return bytes;
}

int main(int argc,char **argv)
{
    // for flags
    (void)argc;
    (void)argv;

    printf(BLUE"Welcome to our PROLAB1's first project 🥳\n");
    printf("Made by Sefa TUNCA and Ardahan AYTAN\n\n"RESET);


    // curl oturumu başlat
    CURL *curl = curl_easy_init();
    if(curl == NULL) // curl oturumu başlatılamadı
    {
        printf(RED"curl failed\n"RESET);
        return 0;
    }
    //set
    curl_easy_setopt(curl, CURLOPT_URL, "http://bilgisayar.kocaeli.edu.tr/prolab1/prolab1.txt");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    //perform
    CURLcode result = curl_easy_perform(curl); // transfer işlemi
    if(result != CURLE_OK)
    {
        printf(RED"İndirme hatası : %s\n"RESET,curl_easy_strerror(result));
        return 0;
    }
    else
    {
        printf(GREEN"İndirme başarılı\n"RESET);
        //printf("%s\n",return_value);
        //printf("%d\n",return_value_size);
    }
    
    char **ret = (char**)malloc(sizeof(char*)*(return_value_size+1));//aldığımız tek boyutlu girdiği strtok ile iki boyutlu hale getiriyoruz
    char *str = strtok(return_value,"\n");
    int i = 0;
    while(str != NULL)
    {
        ret[i] = (char*)malloc(sizeof(char)*(strlen(str)+1));
        strcpy(ret[i],str);
        ret[i][strlen(str)] = '\0';
        i++;
        str = strtok(NULL,"\n");
    }
    ret[i] = NULL;
    //print_array(ret);

    // BURASINDAN SONRA OLDUĞUNDAN PEK EMİN DEĞİLİM

    int girilecek_satir_sayisi = 0;
    printf("Kaç farklı satır girilecek : ");
    scanf("%d",&girilecek_satir_sayisi);
    if((girilecek_satir_sayisi > return_value_size) || girilecek_satir_sayisi <= 0)
    {
        printf(RED"Yanlış farklı satır numarası girişi\n"RESET);
        return 0;
    }       

    printf("Girilecek satırlar : \n");
    int *girilen_satirlar = (int*)malloc(sizeof(int)*(girilecek_satir_sayisi+1));
    for(int i = 0;i<girilecek_satir_sayisi;i++)
    {
        girilen_satirlar[i] = -1; // -1 ile dolduruyorum
    }
    for(int i = 0;i<girilecek_satir_sayisi;i++)
    {
        printf("%d. satır : ",i+1);
        scanf("%d",&girilen_satirlar[i]);
        if(girilen_satirlar[i] > return_value_size || girilen_satirlar[i] <= 0)
        {
            printf(RED"Yanlış satır sayısı girişi\n"RESET);
            return 0;
        }
        if(girilecek_satir_sayisi > 1) // daha önce girilmis mi kontrolü
        {
            for(int j = 0;j<i;j++)
            {
                if(girilen_satirlar[i] == girilen_satirlar[j])
                {
                    printf(RED"Yanlış giriş- Bu karakter daha önce girilmiş!\n"RESET);
                    return 0;
                }
            }
        }
    }
    girilen_satirlar[girilecek_satir_sayisi] = -1; // +1'lik kisim null atilamadigindan böyle yaptim
    
    print_array2(girilen_satirlar);

    printf(GREEN"Girilmesi istenilen satır sayıları sorunsuz bir şekilde alındı!"RESET);


 
    //BURADAN DEVAM EDİLECEK
    
    
    //free
    free(return_value);
    curl_easy_cleanup(curl);
   return (0);
}