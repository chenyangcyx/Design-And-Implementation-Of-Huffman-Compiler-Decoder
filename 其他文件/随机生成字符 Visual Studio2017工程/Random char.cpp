#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#define max 126
#define min 31

void main() {
	int i;
	int num;
	int n;
	char fileadress[100];
	srand(time(0));
	while (1) {
		printf("������ٸ��ַ���  ");
		scanf("%d", &n);
		FILE *text = fopen("text.txt", "w+");
		for (i = 1; i <= n; i++) {
			num = rand() % (max - min) + min;
			if (num == 31)
				fputc(10, text);
			else
				fputc(num, text);
		}
		_getcwd(fileadress, 100);
		printf("����%d���ַ��ɹ���������%s\\text.txt�У�\n\n", n, fileadress);
		fclose(text);
	}
	system("pause");
}