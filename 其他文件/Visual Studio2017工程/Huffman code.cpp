#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>
#include <math.h>
#define OK 1
#define ERROR 0

typedef int Status;

/*����Ԫ�صĶ���*/
typedef struct {
	int char_ASCII;
	int value;
	char *Huffmancode;
	int Huffmancode_bit = 0;
}ElemType;

//����Ķ���
typedef struct node {
	ElemType elem;
	struct node *next;
}LinkNode, *LinkList;

//�����ʼ��
Status List_Init(LinkList &L) {
	if (!(L = (LinkList)malloc(sizeof(LinkNode))))
		exit(ERROR);
	L->next = NULL;
	return OK;
}

//�������
Status List_Insert(LinkList &L, int i, ElemType e) {
	LinkList p, s;
	p = L;
	int j = 0;
	while (p&&j < i - 1) {
		p = p->next;
		j++;
	}
	if (j > i - 1 || !p) return ERROR;
	s = (LinkList)malloc(sizeof(LinkNode));
	s->elem = e;
	s->next = p->next;
	p->next = s;
	return OK;
}

//ɾ������
Status List_Destroy(LinkList &L) {
	LinkList p;
	while (L) {
		p = L;
		L = p->next;
		free(p);
	}
	return OK;
}

/*Huffman���Ķ���*/
typedef struct {
	unsigned int value, ASCII_CODE;
	unsigned int parent, lchid, rchild;
}HTNode, *HuffmanTree;

//��ȡԴ�ļ���Ϣ��ͳ���ַ�����Ƶ��
Status File_sourceload() {
	FILE *fp1 = fopen("text.txt", "r");
	int char_num[256] = { 0 };  //����ASCII����256���ַ�
	int text_length = 0, char_number = 0;
	int i, l = 0;
	char c;

	//��ʼ���ļ��������������˳�
	if (fp1 == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		return (ERROR);
	}

	//ͳ���ַ�Ƶ�ʼ��ַ�������
	while ((c = fgetc(fp1)) != EOF) {
		char_num[c]++;
		text_length++;
	}
	//ͳ���ַ������ж����ֲ�ͬ�ַ�
	for (i = 0; i < 256; i++) {
		if (char_num[i] != 0)
			char_number++;
	}
	fclose(fp1);

	//���д�룺�ַ������ȡ����ֵ��ַ�����
	_mkdir("huffman_temp");
	FILE *fp2 = fopen("huffman_temp\\char_frequency.txt", "w+");
	fprintf(fp2, "%d\t%d", text_length, char_number);
	for (i = 0; i < 256; i++) {
		if (char_num[i] != 0)
			fprintf(fp2, "\n%d %d", i, char_num[i]);
	}
	//д���ı�ע��
	fprintf(fp2, "\n\n****ע�����ļ������洢Դ�ļ��е��ַ��������ַ����������ÿ���ַ���Ӧ��ASCII�뼰���ǵĳ���Ƶ�ʣ�****\n");
	char fileadress[100];
	_getcwd(fileadress, 100);
	printf("�ı��ַ�ͳ����ϣ�\n��Ϣ������%s\\huffman_temp\\char_frequency.txt\n\n", fileadress);
	fclose(fp2);

	//������
	printf("�ı����ȣ�%d    �ַ����ࣺ%d\n", text_length, char_number);
	printf("����ASCII�����У��ַ�����Ƶ�����£�\n");
	for (i = 0; i < 256; i++) {
		if (char_num[i] != 0) {
			if (i == 32)
				printf("�ո�%d\n", char_num[i]);
			else if (i == 13)
				printf("�س���%d\n", char_num[i]);
			else if (i == 10)
				printf("���У�%d\n", char_num[i]);
			else
				printf("%c��%d\t\t", i, char_num[i]);
			l++;
			if (l % 5 == 0)
				printf("\n");
		}
	}
	printf("\n");
	return OK;
}

//��ȡhuffman_temp\char_frequenc.txt�е���Ϣ
Status File_read_char_num(LinkList &L, int &text_length, int &char_number) {
	int i;
	ElemType e;
	List_Init(L);
	FILE *fp = fopen("huffman_temp\\char_frequency.txt", "r");
	fscanf(fp, "%d\t%d", &text_length, &char_number);
	for (i = 1; i <= char_number; i++) {
		fscanf(fp, "\n%d %d", &e.char_ASCII, &e.value);
		List_Insert(L, i, e);
	}
	fclose(fp);
	return OK;
}

//ѡ��Ȩֵ��С�������ڵ�
Status HuffmanTree_Select(HuffmanTree HT, int n, int &s1, int &s2) {
	int i;
	for (i = 1; i <= n; i++) {
		if (HT[i].parent == 0) {
			s1 = i;
			break;
		}
	}
	for (i = 1; i <= n; i++) {
		if (HT[i].value < HT[s1].value&&HT[i].parent == 0)
			s1 = i;
	}
	for (i = 1; i <= n; i++) {
		if (HT[i].parent == 0 && i != s1) {
			s2 = i;
			break;
		}
	}
	for (i = 1; i <= n; i++) {
		if (HT[i].value < HT[s2].value&&HT[i].parent == 0 && i != s1)
			s2 = i;
	}
	return OK;
}

//����Huffman��
Status HuffmanTree_Create(HuffmanTree &HT, int n, LinkList L, int if_print) {
	/*HuffmanTree�ĳ�ʼ��*/
	if (n <= 1) return ERROR;
	int m = 2 * n - 1;
	int i;
	LinkList p = L->next;
	HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
	for (i = 1; i <= m; i++) {
		HT[i].parent = 0;
		HT[i].lchid = 0;
		HT[i].rchild = 0;
	}
	for (i = 1; i <= n; i++) {
		HT[i].value = p->elem.value;
		HT[i].ASCII_CODE = p->elem.char_ASCII;
		p = p->next;
	}
	/*HuffmanTree�Ĵ���*/
	int s1, s2;
	for (i = n + 1; i <= m; i++) {
		HuffmanTree_Select(HT, i - 1, s1, s2);
		HT[s1].parent = i;
		HT[s2].parent = i;
		HT[i].lchid = s1;
		HT[i].rchild = s2;
		HT[i].value = HT[s1].value + HT[s2].value;
	}
	/*���������if_printѡ���Ƿ�Ҫ���������Ϣ*/
	if (if_print == 1) {
		printf("Huffman������Ϣ���£�\n");
		printf("���\t�ַ�\tȨֵ\t˫��\t����\t�Һ���\t\n");
		for (i = 1; i <= m; i++) {
			if (HT[i].ASCII_CODE == 32)
				printf("%d\t�ո�\t%d\t%d\t%d\t%d\t\n", i, HT[i].value, HT[i].parent, HT[i].lchid, HT[i].rchild);
			else if (HT[i].ASCII_CODE == 13)
				printf("%d\t�س�\t%d\t%d\t%d\t%d\t\n", i, HT[i].value, HT[i].parent, HT[i].lchid, HT[i].rchild);
			else if (HT[i].ASCII_CODE == 10)
				printf("%d\t����\t%d\t%d\t%d\t%d\t\n", i, HT[i].value, HT[i].parent, HT[i].lchid, HT[i].rchild);
			else if (HT[i].lchid != 0 || HT[i].rchild != 0)
				printf("%d\t��\t%d\t%d\t%d\t%d\t\n", i, HT[i].value, HT[i].parent, HT[i].lchid, HT[i].rchild);
			else
				printf("%d\t%c\t%d\t%d\t%d\t%d\t\n", i, HT[i].ASCII_CODE, HT[i].value, HT[i].parent, HT[i].lchid, HT[i].rchild);
		}
	}
	return OK;
}

//��Ҷ�ӵ���������ÿ���ַ���Huffman���룬������ָ��L.Huffmancode��
Status HuffmanTree_Code(HuffmanTree HT, int n, LinkList &L) {
	int i, start, c, f;
	LinkList p = L->next;
	char *cd = (char *)malloc(n * sizeof(char));
	cd[n - 1] = '\0';
	for (i = 1; i <= n; i++) {
		start = n - 1;
		for (c = i, f = HT[i].parent; f != 0; c = f, f = HT[f].parent)
			if (HT[f].lchid == c) cd[--start] = '0';
			else cd[--start] = '1';
			p->elem.Huffmancode = (char *)malloc((n - start) * sizeof(char));
			strcpy(p->elem.Huffmancode, &cd[start]);
			p->elem.Huffmancode_bit = n - start - 1;
			p = p->next;
	}
	free(cd);
	return OK;
}

//�洢Huffman����Ϊ�ֵ�
Status HuffmanCode_Write(HuffmanTree HT, int n, LinkList &L) {
	LinkList p = L->next;
	FILE *fp = fopen("huffman_temp\\huffman_codeinfo.txt", "w+");
	int i, line = 0;
	char fileadress[100];
	_getcwd(fileadress, 100);
	printf("Huffman������ϣ�\n��Ϣ������%s\\huffman_temp\\huffman_codeinfo.txt\n", fileadress);
	printf("���ַ���Ӧ��Huffman����Ϊ��\n");
	fprintf(fp, "code number:%d\n", n);
	for (i = 1; i <= n; i++, p = p->next) {
		fprintf(fp, "%d\t%d\t%d\t", p->elem.char_ASCII, p->elem.value, p->elem.Huffmancode_bit);
		if (p->elem.char_ASCII == 32)
			printf("�ո�");
		else if (p->elem.char_ASCII == 13)
			printf("�س���");
		else if (p->elem.char_ASCII == 10)
			printf("���У�");
		else
			printf("%c��", p->elem.char_ASCII);
		p->elem.Huffmancode = &p->elem.Huffmancode[0];
		while (*p->elem.Huffmancode != '\0') {
			fprintf(fp, "%c", *p->elem.Huffmancode);
			printf("%c", *p->elem.Huffmancode);
			p->elem.Huffmancode++;
		}
		line++;  //line������������ĸ�ʽ
		fprintf(fp, "\n");
		printf("\t\t");
		if (line % 4 == 0)
			printf("\n");
	}
	//д���ļ�ע��
	fprintf(fp, "\n****ע�����ļ������洢�����ַ���Ӧ��ASCII���Լ����ǵ�Huffman���룡****\n");
	printf("\n�ļ�д����ϣ�\n");
	fclose(fp);
	List_Destroy(L);
	return OK;
}

//����ĳ���ַ���Huffman�������ڵ�ָ��
char * HuffmanCode_CharPoint(LinkList L, char c) {
	LinkList p = L->next;
	while (p->next&&p->elem.char_ASCII != c)
		p = p->next;
	return p->elem.Huffmancode;
}

//���ļ�����Huffman���� huffman_encode.txt
Status HuffmanCode_Encode(HuffmanTree HT, LinkList L, int if_print) {
	FILE *fp = fopen("huffman_temp\\huffman_codeinfo.txt", "r");
	int char_number, i;
	ElemType e;
	List_Init(L);
	fscanf(fp, "code number:%d\n", &char_number);
	for (i = 1; i <= char_number; i++) {
		fscanf(fp, "%d\t%d\t%d\t", &e.char_ASCII, &e.value, &e.Huffmancode_bit);
		e.Huffmancode = (char*)malloc(e.Huffmancode_bit * sizeof(char));
		fscanf(fp, "%s\n", e.Huffmancode);
		List_Insert(L, i, e);
	}
	char c, *ch;
	FILE *sourcefile = fopen("text.txt", "r");
	FILE *huffman_encode = fopen("huffman_temp\\huffman_encode.txt", "w+");
	if (if_print == 1)
		printf("�ļ���Huffman����Ϊ��\n");
	while (!feof(sourcefile)) {
		c = fgetc(sourcefile);
		ch = HuffmanCode_CharPoint(L, c);
		//д��Huffman 0/1����
		while (*ch != '\0') {
			if (if_print == 1)
				printf("%c", *ch);
			fputc(*ch, huffman_encode);
			ch++;
		}
	}
	//���������if_print�ж��Ƿ�����ļ���Huffman����
	if (if_print == 1)
		printf("\n");
	char fileadress[100];
	_getcwd(fileadress, 100);
	printf("Huffman������ϣ�\n��Ϣ������%s\\huffman_temp\\huffman_encode.txt\n", fileadress);
	fprintf(huffman_encode, "\n\n****ע�����ļ������洢text.txt�ļ���ԭʼHuffman������Ϣ��****\n");
	fclose(fp);
	fclose(sourcefile);
	fclose(huffman_encode);
	List_Destroy(L);
	return OK;
}

//ÿ8λת��Ϊ��Ӧ��ASCII��
char bit_number_transform(int n[]) {
	char num;
	int i;
	for (i = 0; i < 8; i++) {
		//��һ��byte������λ����
		if (n[i] == 1)
			num |= (1 << (7 - i));
		else
			num &= ~(1 << (7 - i));
	}
	return num;
}

//����.huf�ļ�
Status text_huffmancode_create() {
	FILE *fp = fopen("huffman_temp\\huffman_encode.txt", "r");
	FILE *temp = fopen("huffman_temp\\code_readytowrite.txt", "wb+");
	FILE *huf = fopen("text_huffmancode.huf", "wb+");
	int codeinfo[8] = { 0 }, infonum = -1;
	int code_length = 0, code_mod, i, t;
	char c;
	while (!feof(fp)) {
		if (c = fgetc(fp) == '\n')
			break;
		code_length++;
	}
	//������Ҫ��1�ĸ���
	code_mod = code_length % 8;
	fseek(fp, 0L, SEEK_SET);
	for (i = 1; i <= code_length; i++) {
		c = fgetc(fp);
		fputc(c, temp);
	}
	//���ļ����в�1��ʹ���ܳ���Ϊ8��������
	if (code_mod != 0)
		for (i = 1; i <= 8 - code_mod; i++)
			fprintf(temp, "%d", 1);
	fseek(temp, 0L, SEEK_SET);
	fputc(8 - code_mod, huf);
	//���ļ����б���
	for (i = 1; i <= (code_mod == 0 ? code_length : code_length + 8 - code_mod); i++) {
		c = fgetc(temp);
		if (c == '1')
			codeinfo[++infonum] = 1;
		else
			codeinfo[++infonum] = 0;
		if (infonum == 7) {
			c = bit_number_transform(codeinfo);
			fputc(c, huf);
			for (t = 0; t < 8; t++)
				codeinfo[t] = 0;
			infonum = -1;
		}
	}
	char fileadress[100];
	_getcwd(fileadress, 100);
	printf("Huffman��ѹ����ϣ�\n��Ϣ������%s\\text_huffmancode.huf��\n", fileadress);
	fclose(fp);
	fclose(temp);
	fclose(huf);
	return OK;
}

//��ѹ.huf�ļ�
Status text_huffmancode_unzip() {
	FILE *huf = fopen("text_huffmancode.huf", "rb");
	FILE *unzip = fopen("huffman_temp\\huf_decode.txt", "wb+");
	long huflength = 0;
	double code_mod = 0.0;
	char c;
	int i, t;
	long count = 0;
	//��ȡ�ļ�����
	fseek(huf, 0L, SEEK_SET);
	fseek(huf, 0L, SEEK_END);
	huflength = ftell(huf);
	//���¶�λ�ļ�ָ�룬��ʼ��ȡ
	fseek(huf, 0L, SEEK_SET);
	c = fgetc(huf);
	//��ȡhuf�ļ��ĵ�һ���ַ�������õ���1�ĸ���
	for (i = 0; i <= 4; i++) {
		code_mod += ((c >> i) & 1)*pow(2, i);
	}
	//��huf�ļ���������
	while (!feof(huf)) {
		c = fgetc(huf);
		for (t = 0; t < 8; t++) {
			if (((c >> (7 - t)) & 1) == 1) {
				count++;
				fputc(49, unzip);
				if (count == (8 * (huflength - 1) - code_mod))
					break;
			}
			else if (((c >> (7 - t)) & 1) == 0) {
				count++;
				fputc(48, unzip);
				if (count == (8 * (huflength - 1) - code_mod))
					break;
			}
		}
		if (count == (8 * (huflength - 1) - code_mod))
			break;
	}
	char fileadress[100];
	_getcwd(fileadress, 100);
	printf("huf�ļ�������ϣ�\n��Ϣ������%s\\huffman_temp\\huf_decode.txt��\n", fileadress);
	fclose(huf);
	fclose(unzip);
	return OK;
}

//���ļ�����Huffman���� text_decode.txt
Status HuffmanCode_FileDecode(HuffmanTree HT, int char_number, int text_length) {
	FILE *encodefile = fopen("huffman_temp\\huf_decode.txt", "r");
	FILE *decodefile = fopen("text_decode.txt", "w+");
	int q = 2 * char_number - 1;
	int k = 0, word = 0;
	char c;
	//�Ӹ���ʼ������Ѱ��Huffman�����Ӧ���ַ�
	while (!feof(encodefile)) {
		c = fgetc(encodefile);
		if (word == text_length)
			break;
		if (c == '0')
			q = HT[q].lchid;
		else if (c == '1')
			q = HT[q].rchild;
		if (HT[q].lchid == 0 && HT[q].rchild == 0) {
			fputc(HT[q].ASCII_CODE, decodefile);
			q = 2 * char_number - 1;
			word++;
		}
	}
	char fileadress[100];
	_getcwd(fileadress, 100);
	printf("ѹ���ļ�������ϣ�\n��Ϣ������%s\\text_decode.txt�У�\n", fileadress);
	fclose(encodefile);
	fclose(decodefile);
	return OK;
}

//�������ļ�������ļ����бȽϣ�����ȷ��
Status File_Error_Percentage() {
	int i;
	char ch1, ch2;
	float text_length = 3894.0, error_num = 0.0, percentage = 0.0;
	FILE *source = fopen("text.txt", "r");
	FILE *decode = fopen("text_decode.txt", "r");
	//����ַ����бȽϣ���һ���������һ������������һ���ַ�
	for (i = 0; i <= text_length; i++) {
		ch1 = fgetc(source);
		ch2 = fgetc(decode);
		if (ch1 != ch2)
			error_num++;
	}
	percentage = (text_length - error_num) / text_length;
	printf("Դ�ļ��������ļ���ȣ��������Ϊ��%d��������Ϊ��%.2f%%\n", (int)error_num, percentage * 100);
	return OK;
}

//����ѹ����
Status File_Zip_Percentage() {
	long text_size, text_decode_size;
	float zippercentage;
	FILE *source = fopen("text.txt", "r");
	FILE *decode = fopen("text_huffmancode.huf", "r");
	//��ȡ�ļ�text.txt�Ĵ�С
	fseek(source, 0L, SEEK_END);
	text_size = ftell(source);
	//��ȡ�ļ�text_huffmancode.huf�Ĵ�С
	fseek(decode, 0L, SEEK_END);
	text_decode_size = ftell(decode);
	zippercentage = (float)text_decode_size / (float)text_size;
	printf("Դ�ļ���СΪ��%d�ֽ�  ѹ�����СΪ��%d�ֽ�\nѹ����Ϊ��%.2f%%\n", text_size, text_decode_size, zippercentage*100.0);
	return OK;
}

//��ʾ�˵�
void menu() {
	printf("  ----------------------------------------------------\n");
	printf(" |                                                    |\n");
	printf(" |            Huffman����/�������                    |\n");
	printf(" |                                                    |\n");
	printf(" |----���ִ��----------------------------------------|\n");
	printf(" |                                                    |\n");
	printf(" |              1.������ִ�����в���                  |\n");
	printf(" |                                                    |\n");
	printf(" |----����ִ��----------------------------------------|\n");
	printf(" |                                                    |\n");
	printf(" |              2.ͳ���ַ�Ƶ��                        |\n");
	printf(" |              3.����Huffman��                       |\n");
	printf(" |              4.��ÿ���ַ���Huffman����             |\n");
	printf(" |              5.���ļ�����Huffman����               |\n");
	printf(" |              6.���ݱ�������.huf�ļ�                |\n");
	printf(" |              7.����Huffmanѹ����                   |\n");
	printf(" |              8.���ݱ��뷭��.huf�ļ�                |\n");
	printf(" |              9.�Է���õ����ļ����н���            |\n");
	printf(" |             10.�Ƚ������ļ�������ļ�              |\n");
	printf(" |                                                    |\n");
	printf(" |             11.�˳�����                            |\n");
	printf(" |                                                    |\n");
	printf("  ----------------------------------------------------\n");
	int choice = 0;
	while (choice<1 || choice>11)
	{
		printf("��ѡ����Ҫִ�еĶ�����  ");
		scanf("%d", &choice);
		if (choice<1 || choice>11)
			printf("\n�Բ��������������������룡\n\n");
	}
	//�Ժ����в˵�����������
	void menu_function1();
	void menu_function2();
	void menu_function3();
	void menu_function4();
	void menu_function5();
	void menu_function6();
	void menu_function7();
	void menu_function8();
	void menu_function9();
	void menu_function10();
	void menu_function11();
	switch (choice)
	{
	case 1:menu_function1(); break;
	case 2:menu_function2(); break;
	case 3:menu_function3(); break;
	case 4:menu_function4(); break;
	case 5:menu_function5(); break;
	case 6:menu_function6(); break;
	case 7:menu_function7(); break;
	case 8:menu_function8(); break;
	case 9:menu_function9(); break;
	case 10:menu_function10(); break;
	case 11:menu_function11(); break;
	}
}

//ѯ���Ƿ������һ������
int if_go_next() {
	int c = 2;
	while (c != 1 && c != 0) {
		printf("�Ƿ�Ҫ������һ����������1=��  0=��    ");
		scanf("%d", &c);
		getchar();
		if (c != 1 && c != 0)
			printf("\n�����������������룡\n");
	}
	if (c == 1) {
		system("cls");
		return 1;
	}
	else return 0;
}

//ѯ���Ƿ񷵻ز˵�
void if_backto_menu() {
	int c = 2;
	while (c != 1 && c != 0) {
		printf("�Ƿ�Ҫ���ز˵�����1=��  0=��    ");
		scanf("%d", &c);
		getchar();
		if (c != 1 && c != 0)
			printf("\n�����������������룡\n");
	}
	if (c == 1) {
		system("cls");
		menu();
	}
	else exit(OK);
}

//�˵�����1
void menu_function1() {
	LinkList L;
	HuffmanTree HT;
	int text_length, char_number;
	int whether = 0;

	printf("\n��ѡ�����1��\n");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�1����ͳ���ַ�Ƶ�ʣ�\n");
	FILE *fp1 = fopen("text.txt", "r");
	if (fp1 == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	File_sourceload();
	printf("\nͳ���ַ���ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�2��������Huffman����\n");
	printf("�ڴ���֮���Ƿ�Ҫ�鿴Huffman������1=�鿴  0=���鿴��  ");
	scanf("%d", &whether);
	File_read_char_num(L, text_length, char_number);
	HuffmanTree_Create(HT, char_number, L, whether);
	printf("\n����Huffman����ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�3������ÿ���ַ���Huffman���룡\n");
	HuffmanTree_Code(HT, char_number, L);
	HuffmanCode_Write(HT, char_number, L);
	printf("\n�ַ�Huffman����������ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�4�������ļ�����Huffman���룡\n");
	printf("�ڴ���֮���Ƿ�Ҫ�鿴�ļ���Huffman���룿��1=�鿴  0=���鿴��  ");
	scanf("%d", &whether);
	HuffmanCode_Encode(HT, L, whether);
	printf("\n�ļ�Huffman������ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�5�������ݱ�������.huf�ļ���\n");
	text_huffmancode_create();
	printf("\n����.huf�ļ���ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�6��������Huffmanѹ���ʣ�\n");
	File_Zip_Percentage();
	printf("\n����Huffmanѹ������ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�7�������ݱ��뷭��.huf�ļ���\n");
	text_huffmancode_unzip();
	printf("\n���ݱ��뷭��.huf�ļ���ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�8�����Է���õ����ļ����н��룡\n");
	HuffmanCode_FileDecode(HT, char_number, text_length);
	printf("\n���ļ����н�����ϣ�");
	if (if_go_next() == 0)
		if_backto_menu();

	printf("\n��ʼ���е�9�����Ƚ������ļ�������ļ���\n");
	File_Error_Percentage();

	printf("\n���в���ִ����ϣ�\n");
	if_backto_menu();
}

//�˵�����2
void menu_function2() {
	printf("\n��ѡ�����2����ȷ�������ļ�text.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *fp1 = fopen("text.txt", "r");
	if (fp1 == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(fp1);
	printf("\n��ʼͳ���ַ�Ƶ�ʣ�\n");
	File_sourceload();
	printf("\nͳ���ַ���ϣ�\n");
	if_backto_menu();
}

//�˵�����3
void menu_function3() {
	printf("\n��ѡ�����3����ȷ�������ļ�huffman_temp\\char_frequency.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *fp = fopen("huffman_temp\\char_frequency.txt", "r");
	if (fp == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(fp);
	printf("\n��ʼ����Huffman����\n");
	int whether = 0;
	printf("�ڴ���֮���Ƿ�Ҫ�鿴Huffman������1=�鿴  0=���鿴��  ");
	scanf("%d", &whether);
	LinkList L;
	HuffmanTree HT;
	int text_length, char_number;
	File_read_char_num(L, text_length, char_number);
	HuffmanTree_Create(HT, char_number, L, whether);
	printf("\n����Huffman����ϣ�\n");
	if_backto_menu();
}

//�˵�����4
void menu_function4() {
	printf("\n��ѡ�����4����ȷ�������ļ�huffman_temp\\char_frequency.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *fp = fopen("huffman_temp\\char_frequency.txt", "r");
	if (fp == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(fp);
	LinkList L;
	HuffmanTree HT;
	int text_length, char_number;
	File_read_char_num(L, text_length, char_number);
	HuffmanTree_Create(HT, char_number, L, 0);
	HuffmanTree_Code(HT, char_number, L);
	HuffmanCode_Write(HT, char_number, L);
	printf("\n��ÿ���ַ���Huffman������ϣ�\n");
	if_backto_menu();
}

//�˵�����5
void menu_function5() {
	printf("\n��ѡ�����5����ȷ�������ļ�huffman_temp\\huffman_codeinfo.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *fp = fopen("huffman_temp\\huffman_codeinfo.txt", "r");
	if (fp == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(fp);
	int whether = 0;
	LinkList L;
	HuffmanTree HT;
	printf("�ڴ���֮���Ƿ�Ҫ�鿴�ļ���Huffman���룿��1=�鿴  0=���鿴��  ");
	scanf("%d", &whether);
	int text_length, char_number;
	File_read_char_num(L, text_length, char_number);
	HuffmanTree_Create(HT, char_number, L, 0);
	HuffmanCode_Encode(HT, L, whether);
	printf("\n���ļ�����Huffman������ϣ�\n");
	if_backto_menu();
}

//�˵�����6
void menu_function6() {
	printf("\n��ѡ�����6����ȷ�������ļ�huffman_temp\\huffman_encode.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *fp = fopen("huffman_temp\\huffman_encode.txt", "r");
	if (fp == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(fp);
	text_huffmancode_create();
	printf("\n����.huf�ļ���ϣ�\n");
	if_backto_menu();
}

//�˵�����7
void menu_function7() {
	printf("\n��ѡ�����7��\n��ȷ�������ļ�text.txt��text_huffmancode.huf��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *source = fopen("text.txt", "r");
	FILE *decode = fopen("text_huffmancode.huf", "r");
	if (source == NULL || decode == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(source);
	fclose(decode);
	File_Zip_Percentage();
	printf("\n����Huffmanѹ������ϣ�\n");
	if_backto_menu();
}

//�˵�����8
void menu_function8() {
	printf("\n��ѡ�����8��\n��ȷ�������ļ�text_huffmancode.huf��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *huf = fopen("text_huffmancode.huf", "rb");
	if (huf == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(huf);
	text_huffmancode_unzip();
	printf("\n���ݱ��뷭��.huf�ļ���ϣ�\n");
	if_backto_menu();
}

//�˵�����9
void menu_function9() {
	printf("\n��ѡ�����9��\n��ȷ�������ļ�huffman_temp\\huf_decode.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *encodefile = fopen("huffman_temp\\huf_decode.txt", "r");
	if (encodefile == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(encodefile);
	int text_length, char_number;
	HuffmanTree HT;
	LinkList L;
	File_read_char_num(L, text_length, char_number);
	HuffmanTree_Create(HT, char_number, L, 0);
	HuffmanCode_FileDecode(HT, char_number, text_length);
	printf("\n���ļ����н�����ϣ�\n");
	if_backto_menu();
}

//�˵�����10
void menu_function10() {
	printf("\n��ѡ�����10��\n��ȷ�������ļ�text.txt��text_decode.txt��\n");
	if (if_go_next() == 0)
		if_backto_menu();
	FILE *decode = fopen("text_decode.txt", "r");
	if (decode == NULL) {
		printf("��ȡ�ļ�ʧ�ܻ��ļ������ڣ������ԣ�\n");
		if_backto_menu();
	}
	fclose(decode);
	File_Error_Percentage();
	printf("\n�����ļ�������ļ��Ƚ���ϣ�\n");
	if_backto_menu();
}

//�˵�����11
void menu_function11() {
	exit(OK);
}

//����ָ������ѡ������ֱ��������ȡ�ַ�Ƶ�ʣ�����ִ��
void Project_Recovery() {
	int whether = 0;
	LinkList L;
	HuffmanTree HT;
	int text_length, char_number;
	printf("�Ƿ����ִ�и����񣿣�1=��  0=��    ");
	scanf("%d", &whether);
	if (whether == 1) {
		File_sourceload();
		printf("�Ѿ����¼��ظ�����\n");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�2��������Huffman����\n");
		printf("�ڴ���֮���Ƿ�Ҫ�鿴Huffman������1=�鿴  0=���鿴��  ");
		scanf("%d", &whether);
		File_read_char_num(L, text_length, char_number);
		HuffmanTree_Create(HT, char_number, L, whether);
		printf("\n����Huffman����ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�3������ÿ���ַ���Huffman���룡\n");
		HuffmanTree_Code(HT, char_number, L);
		HuffmanCode_Write(HT, char_number, L);
		printf("\n�ַ�Huffman����������ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�4�������ļ�����Huffman���룡\n");
		printf("�ڴ���֮���Ƿ�Ҫ�鿴�ļ���Huffman���룿��1=�鿴  0=���鿴��  ");
		scanf("%d", &whether);
		HuffmanCode_Encode(HT, L, whether);
		printf("\n�ļ�Huffman������ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�5�������ݱ�������.huf�ļ���\n");
		text_huffmancode_create();
		printf("\n����.huf�ļ���ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�6��������Huffmanѹ���ʣ�\n");
		File_Zip_Percentage();
		printf("\n����Huffmanѹ������ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�7�������ݱ��뷭��.huf�ļ���\n");
		text_huffmancode_unzip();
		printf("\n���ݱ��뷭��.huf�ļ���ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�8�����Է���õ����ļ����н��룡\n");
		HuffmanCode_FileDecode(HT, char_number, text_length);
		printf("\n���ļ����н�����ϣ�");
		if (if_go_next() == 0)
			if_backto_menu();

		printf("\n��ʼ���е�9�����Ƚ������ļ�������ļ���\n");
		File_Error_Percentage();

		printf("\n���в���ִ����ϣ�\n");
		if_backto_menu();
	}
	else {
		system("cls");
		menu();
	}
}

void main() {
	FILE *fp = fopen("huffman_temp\\char_frequency.txt", "r");
	//�ж��Ƿ����δ�������
	if (fp != NULL) {
		printf("������һ�γ�������ʱδ����������\n");
		Project_Recovery();
	}
	else
		menu();
	system("pause");
}