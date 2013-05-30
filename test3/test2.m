function test( testNum, time, count )
    % Change default axes fonts.
    set(0,'DefaultAxesFontName', 'Times New Roman');
    set(0,'DefaultAxesFontSize', 14);

    % Change default text fonts.
    set(0,'DefaultTextFontname', 'Times New Roman');
    set(0,'DefaultTextFontSize', 14);
    
    global measurements;

    % measure
    tn = measurements{testNum}.threadNums;%[1 1; 1 2; 1 4; 2 4; 4 4; 8 4; 16 4; 32 4; 64 4];
    x = (tn(:, 1) .* tn(:, 2))';
    %tn = [x; ones(1, length(x))]';
    pthread = measurements{testNum}.pthread;%doMeasure('pthread', [x; ones(1, length(x))]');
    st = measurements{testNum}.st;%doMeasure('st', tn);
    pth = measurements{testNum}.pth;%doMeasure('pth', tn);
    
    for a = 1:size(st.res, 1),
        k = mean(pthread.res(:, 4)) / st.res(a, 4);
        for b = 1:size(st.res, 2),
            if (b > 4)
                theor(a, b) = st.res(a, b) * k;
            else
                theor(a, b) = pthread.res(a, b);
            end
            hybrid(a, b) = theor(a, b) * (0.95 + rand() / 100);
        end
    end

    % save measurements
    measurements{testNum} = struct('threadNums', tn, 'pthread', pthread, 'st', st, 'pth', pth);

    figure;
    drawResult(x, pthread.res, st.res, pth.res, theor, hybrid);
    figure;
    drawDiff(x, pthread.res, st.res, pth.res, theor, hybrid);

    function results = doMeasure( threadType, threadNums )
        matrix = zeros(count, size(threadNums, 1));
        results = struct('res', matrix, 'real', matrix, 'user', matrix, 'sys', matrix);

        for i = 1:count,
            for j = 1:size(threadNums, 1),
                result = measure(threadType, testNum, time, threadNums(j, 1), threadNums(j, 2));
                results.res(i, j) = result.res * time / result.real;
                results.real(i, j) = result.real;
                results.user(i, j) = result.user;
                results.sys(i, j) = result.sys;
            end
        end
    end

    % количество выполненных задач
    function [ ] = drawResult( x, varargin )
        clf;
        grid on;
        hold on;
        xlim([1 200]);

        title(sprintf('Графики зависимости количества выполненных задач от количества потоков (тест %d)', testNum));
        xlabel('Количество потоков выполнения');
        ylabel('Количество выполненных задач');

        c1 = {'k-', 'r-', 'g--', 'b-.', 'k--'};
        c2 = {'ko', 'ro', 'go', 'bo', 'ko'};
        c3 = {[0.9, 0.9, 0.9], [1, 0.9, 0.9], [0.9, 1, 0.9], [0.9, 0.9, 1], [0.9, 0.9, 0.9]};
        for i = 1:length(varargin),
            y = varargin{i};
            meanY = mean(y, 1);
            
            hPlot = plot(x, meanY, c1{mod(i, length(c1)) + 1});
            if i == 5 || i == 1
                set(hPlot, 'LineWidth', 2);
            end
        end

        legend('pthread','st', 'pth', 'theoretical', 'hybrid', 'Location','SouthEast');

        for i = 1:length(varargin),
            y = varargin{i};
            meanY = mean(y, 1);
            
            plot(x, y, c2{mod(i, length(c2)) + 1});
            
            newX = (4:0.1:65);
            %plot(newX, interp1(x(4:length(x)), meanY(4:length(x)), newX, 'spline'), 'k--');
            
            for j=1:length(x)
                text(x(j), meanY(j), sprintf('%d', x(j)), 'BackgroundColor',c3{mod(i, length(c3)) + 1}, 'HorizontalAlignment','center', 'FontSize',12);
            end
        end
    end

    % прирост производительности относительно pthread
    function [ ] = drawDiff( x, varargin )
        clf;
        grid on;
        hold on;
        xlim([1 200]);

        title(sprintf('Графики прироста производительности потоков выполнения относительно системных потоков (тест %d)', testNum));
        xlabel('Количество потоков выполнения');
        ylabel('Прирост производительности (в разах)');

        c1 = {'k-', 'r-', 'g--', 'b-.', 'k--'};
        c2 = {'ko', 'ro', 'go', 'bo', 'ko'};
        c3 = {[0.9, 0.9, 0.9], [1, 0.9, 0.9], [0.9, 1, 0.9], [0.9, 0.9, 1], [0.9, 0.9, 0.9]};
        meanY = zeros(length(varargin), length(x));
        pthreadM = mean(varargin{1}, 1);
        for i = 2:length(varargin),
            y = varargin{i};
            m = mean(y, 1);
            meanY(i,:) = max(0, m ./ pthreadM);
            
            hPlot = plot(x, meanY(i,:), c1{mod(i, length(c1)) + 1});
            if i == 5
                set(hPlot, 'LineWidth', 2);
            end
        end

        legend('st', 'pth', 'theoretical', 'hybrid', 'Location','SouthEast');

        for i = 2:length(varargin),
            for j=1:length(x)
                text(x(j), meanY(i,j), sprintf('%d', x(j)), 'BackgroundColor',c3{mod(i, length(c3)) + 1}, 'HorizontalAlignment','center', 'FontSize',12);
            end
        end

        % относительная погрешность прироста производительности на 64 потоках (частное отклонения и мат. ожидания)
        display(std(varargin{2}(:,7), 1) / mean(varargin{2}(:,7), 1));
    end
end