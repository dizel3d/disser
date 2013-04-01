function [ ] = test_f( x, varargin )
clf;
grid on;
hold on;
xlim([1 65]);

c1 = {'r-', 'g--', 'b-.'};
c2 = {'ro', 'go', 'bo'};
c3 = {[1, 0.9, 0.9], [0.9, 1, 0.9], [0.9, 0.9, 1]};
for i = 1:length(varargin),
    y = varargin{i};
    meanY = mean(y);
    
    plot(x, meanY, c1{mod(i, length(c1)) + 1});
end

legend('pthread','st', 'pth', 'Location','SouthEast');

for i = 1:length(varargin),
    y = varargin{i};
    meanY = mean(y);
    
    plot(x, y, c2{mod(i, length(c2)) + 1});
    
    newX = (4:0.1:65);
    plot(newX, interp1(x(4:10), meanY(:,4:10), newX, 'spline'), 'k--');
    
    for j=1:length(x)
        text(x(j), meanY(j), sprintf('%d', x(j)), 'BackgroundColor',c3{mod(i, length(c3)) + 1}, 'HorizontalAlignment','center', 'FontSize',12);
    end
end

end

