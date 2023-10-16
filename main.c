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

struct nokta
{
    int x;
    int y;
    struct nokta *next;
};

struct satir
{
    char *satir_data;
    int satir_no;
    struct nokta *nokta_listesi;
    struct satir *next;
};

void liste_sona_ekleme1(struct nokta **nokta_listesi,struct nokta *nokta_node)
{
    struct nokta *s;
    
    s = *nokta_listesi;
    if(s == NULL)
    {
        *nokta_listesi = nokta_node;
        nokta_node->next = NULL;
        return;
    }
    while(s->next != NULL)
    {
        s = s->next;
    }
    s->next = nokta_node;
    nokta_node->next = NULL;
}

void liste_sona_ekleme2(struct satir **satir_listesi,struct satir *satir_node)
{
    struct satir *s;
    
    s = *satir_listesi;
    if(s == NULL)
    {
        *satir_listesi = satir_node;
        satir_node->next = NULL;
        return;
    }
    while(s->next != NULL)
    {
        s = s->next;
    }
    s->next = satir_node;
    satir_node->next = NULL;
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

    //BURADAN DEVAM EDİLECEK - BURADAN BAŞLANILDI
    printf(GREEN"Girilmesi istenilen satır sayıları sorunsuz bir şekilde alındı!\n"RESET);


 
    


    //satirin ilk elamanını oluştur
    int satir_size = 0;
    while(satir_size < return_value_size)
    {
        struct satir *ilk_satir = (struct satir*)malloc(sizeof(struct satir));
        ilk_satir->satir_data = (char*)malloc(sizeof(char)*(strlen(ret[satir_size])-2));
        int idx = 0;
        for(size_t i = 2;i<strlen(ret[0])-1;i++)
        {
            ilk_satir->satir_data[idx] = ret[0][i];
            idx++;
        }
        ilk_satir->satir_data[strlen(ret[0])] = '\0';
        ilk_satir->satir_no = satir_size+1;
        ilk_satir->nokta_listesi = NULL;
        ilk_satir->next = NULL;

        int nokta_sayisi = 0;
        int idx0 = 0;
        while(ilk_satir->satir_data[idx0] != '\0')
        {
            if(ilk_satir->satir_data[idx0] == '(')
            {
                nokta_sayisi++;
            }
            idx0++;
        }
         if(nokta_sayisi == 0)
        {
            ilk_satir->nokta_listesi = NULL;
        }
        char **nokta_split = (char**)malloc(sizeof(char*)*(nokta_sayisi+1));
        char *temp = strdup(ilk_satir->satir_data);  // ana structtaki datanın      bozulmaması için strtok işleminden sonra bozuluyordu
        char *tok = strtok(temp,"(");
        int i1 = 0;
        while(tok != NULL)
        {
            size_t s = 0;
            nokta_split[i1] = (char*)malloc(sizeof(char)*(strlen(tok)));
            strcpy(nokta_split[i1],tok);
            for(;tok[s] != ')';s++)
            {
                nokta_split[i1][s] = tok[s];
            }
            nokta_split[i1][s] = '\0';

            //printf("%d - %s\n",i,nokta_split[i]);
            i1++;
            tok = strtok(NULL,"(");
        }
        nokta_split[i1] = NULL;
        print_array(nokta_split);
        printf("\n");
        int id = 0;
        while(id < nokta_sayisi)
        {
            int temp_x = 0;
            int temp_y = 0;
            int id_ = 0;
            struct nokta *nokta = (struct nokta*)malloc (sizeof(struct nokta));
            while(nokta_split[id][id_] != ',')
            {
                temp_x = temp_x*10 + (nokta_split[id][id_] - '0');
                id_++;
            }
            id_++;
            while(nokta_split[id][id_] != '\0')
            {
                temp_y = temp_y*10 + (nokta_split[id][id_] - '0');
                id_++;
            }
            nokta->x = temp_x;
            nokta->y = temp_y;
            liste_sona_ekleme1(&ilk_satir->nokta_listesi, nokta);
            id++;
        }
        liste_sona_ekleme2(&ilk_satir,ilk_satir);
        satir_size++; 
    }
    
    //return value'nun 2.indexsinden başlayıp sondan bir öncek indexi alma
    



    
    
    //free
    free(return_value);
    curl_easy_cleanup(curl);
   return (0);
}