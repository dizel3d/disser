x = [1 2 3 4 8 16 32 64 128 256];
y1 = [
    416.2 774.4 1052.8 1225.2 1048.6 930.6 840.2 767.6 757.4 747.6;
    432.8 764 999.6 1225 1049.6 938.4 847.6 777.6 730.4 742.6;
    435.2 761.4 1050.8 1230.8 1047.4 929.2 847.6 770.6 742.4 715;
    415.4 768 1049.2 1227.2 1047.4 929.8 835.2 769.8 763 740;
    425.2 765.2 1048.8 1229.4 1045.8 928.6 840.8 774 745.6 745.8
];
y2 = [
    420.6 723.8 1000.2 1204.4 1218.8 1212.8 1209.6 1206.4 1177.6 1126.4;
    421 724.8 1002.4 1200.2 1218.4 1212.8 1214.4 1200 1145.6 1126.4;
    422.4 718 1002.6 1196.8 1180 1219.2 1212.8 1203.2 1184 1126.4;
    413.2 716 1001.8 1207.4 1207.6 1218.4 1185.6 1206.4 1177.6 1126.4;
    421.2 723.4 1004.2 1208 1208.8 1218.4 1214.4 1209.6 1177.6 1126.4
];

clf;
grid on;
hold on;
axis([1 65 0 1300]);

meanY1 = mean(y1);
meanY2 = mean(y2);

plot(x, y1, 'ro');
plot(x, y2, 'ko');
plot(x, meanY1, 'ro-', 'MarkerFaceColor','r');
plot(x, meanY2, 'ko-', 'MarkerFaceColor','k');

newX = (4:0.1:65);
plot(newX, interp1(x(4:10), meanY1(:,4:10), newX, 'spline'), 'r--');
plot(newX, interp1(x(4:10), meanY2(:,4:10), newX, 'spline'), 'k--');
newX = (1:0.1:4);
plot(newX, interp1(x(1:4), meanY1(:,1:4), newX, 'spline'), 'r--');
plot(newX, interp1(x(1:4), meanY2(:,1:4), newX, 'spline'), 'k--');

for i=1:length(x)
    text(x(i), meanY1(i), sprintf('%d', x(i)), 'BackgroundColor',[.9 .7 .7], 'HorizontalAlignment','center');
    text(x(i), meanY2(i), sprintf('%d', x(i)), 'BackgroundColor',[.8 .9 .9], 'HorizontalAlignment','center');
end;
