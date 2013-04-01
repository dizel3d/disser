function [ ] = test_f2( x, varargin )
clf;
grid on;
hold on;
xlim([1 65]);

c1 = {'r-', 'g--', 'b-.'};
c2 = {'ro', 'go', 'bo'};
c3 = {[1, 0.9, 0.9], [0.9, 1, 0.9], [0.9, 0.9, 1]};
meanY = zeros(length(varargin), length(x));
pthreadM = mean(varargin{1});
for i = 2:length(varargin),
    y = varargin{i};
    m = mean(y);
    meanY(i,:) = max(0, m ./ pthreadM);
    
    plot(x, meanY(i,:), c1{mod(i, length(c1)) + 1});
end

legend('st', 'pth', 'Location','SouthEast');

for i = 2:length(varargin),
    for j=1:length(x)
        text(x(j), meanY(i,j), sprintf('%d', x(j)), 'BackgroundColor',c3{mod(i, length(c3)) + 1}, 'HorizontalAlignment','center', 'FontSize',12);
    end
end

display(std(varargin{2}(:,7), 1) / mean(varargin{2}(:,7)));

end

