clear
clc
close

%Read data files and make plots
a=dir('datafiles');
for i=3:(size(a,1)),
    data=importdata(['datafiles\' a(i).name]);
    figure(i)
    plot(data(:,1),data(:,2))
end