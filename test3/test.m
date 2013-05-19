function test( testNum, time, count )
    % Change default axes fonts.
    set(0,'DefaultAxesFontName', 'Times New Roman');
    set(0,'DefaultAxesFontSize', 14);

    % Change default text fonts.
    set(0,'DefaultTextFontname', 'Times New Roman');
    set(0,'DefaultTextFontSize', 14);

    threadNums = [1 1; 1 2; 1 4; 2 4; 4 4; 8 4; 16 4; 32 4; 64 4];

    % measure
    x = (threadNums(:, 1) .* threadNums(:, 2))';
    pthread = doMeasure('pthread');
    st = doMeasure('pth');
    pth = doMeasure('st');

    % save measurements
    global measurements;
    measurements{testNum} = struct('threadNums', threadNums, 'pthread', pthread, 'st', st, 'pth', pth);

    figure;
    drawResult(x, pthread.res, st.res, pth.res);
    figure;
    drawDiff(x, pthread.res, st.res, pth.res);

    function results = doMeasure( threadType )
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

        c1 = {'r-', 'g--', 'b-.'};
        c2 = {'ro', 'go', 'bo'};
        c3 = {[1, 0.9, 0.9], [0.9, 1, 0.9], [0.9, 0.9, 1]};
        for i = 1:length(varargin),
            y = varargin{i};
            meanY = mean(y, 1);
            
            plot(x, meanY, c1{mod(i, length(c1)) + 1});
        end

        legend('pthread','st', 'pth', 'Location','SouthEast');

        for i = 1:length(varargin),
            y = varargin{i};
            meanY = mean(y, 1);
            
            plot(x, y, c2{mod(i, length(c2)) + 1});
            
            newX = (4:0.1:65);
            plot(newX, interp1(x(4:length(x)), meanY(4:length(x)), newX, 'spline'), 'k--');
            
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
        xlim([1 128]);

        title(sprintf('Графики прироста производительности потоков выполнения относительно системных потоков (тест %d)', testNum));
        xlabel('Количество потоков выполнения');
        ylabel('Прирост производительности (в разах)');

        c1 = {'r-', 'g--', 'b-.'};
        c2 = {'ro', 'go', 'bo'};
        c3 = {[1, 0.9, 0.9], [0.9, 1, 0.9], [0.9, 0.9, 1]};
        meanY = zeros(length(varargin), length(x));
        pthreadM = mean(varargin{1}, 1);
        for i = 2:length(varargin),
            y = varargin{i};
            m = mean(y, 1);
            meanY(i,:) = max(0, m ./ pthreadM);
            
            plot(x, meanY(i,:), c1{mod(i, length(c1)) + 1});
        end

        legend('st', 'pth', 'Location','SouthEast');

        for i = 2:length(varargin),
            for j=1:length(x)
                text(x(j), meanY(i,j), sprintf('%d', x(j)), 'BackgroundColor',c3{mod(i, length(c3)) + 1}, 'HorizontalAlignment','center', 'FontSize',12);
            end
        end

        % относительная погрешность прироста производительности на 64 потоках (частное отклонения и мат. ожидания)
        display(std(varargin{2}(:,7), 1) / mean(varargin{2}(:,7), 1));
    end
end