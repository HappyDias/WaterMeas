if exist('s','var'), fclose(s); end
s = serial('COM3','BaudRate',9600);
s.InputBufferSize=1024;
fopen(s);
%b=char(fread(s,1,'uint8'))
c=char(fread(s,1,'uint8'));
x=1:10000;
y=zeros(1,10000);
count = 1;
figure(1);
ylim([0,1000]);
hold on;
h = [];
while c~='|',
    c=char(fread(s,1,'uint8'));
end
while 1,
    strNum=[];
    c=char(fread(s,1,'uint8'));
    while c~='|',    
        strNum=[strNum c];
        c = char(fread(s,1,'uint8'));
    end
    y = [str2num(strNum) y];
    y = y(1:end-1);
    count = count + 1;
    if count > 7
        count = 1;
        delete(h);
        h = plot(x, y);
        drawnow;
    end
end
fclose(s)