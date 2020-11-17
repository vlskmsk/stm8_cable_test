function Read_Pressure_data(port)
    close all;
    if ~isempty(instrfind)
        fclose(instrfind);
        delete(instrfind);
    end
    fmt = 'HH:mm:ss.SSS';
    time = datestr(now, 'dd-mmm-yyyy HH:MM:SS');
    time(12) = '-';
    time(15) = '-';
    time(18) = '-';
    filename=['pressure_sensor','-',time,'.csv'];
    A = 'Time,Sensor 1,Sensor 2,Sensor 3,Sensor 4';
    fid = fopen(filename,'w'); 
    fprintf(fid,'%s\n',A);
    s=serial(port);
    set(s,'BaudRate',115200);
    fopen(s);
    figure(1);
    state = 0;
    conv = 1/385.5;
    numbytes = 8;
    pressure = zeros(1, 4);
    val_list={[0],[0],[0],[0]};
    counter = 1;
    x = 1:1:202;
    for i=1:4
        ax(i) = subplot(2,2,i);
        str=['sensor ',num2str(i)];
        title(ax(i),str);
        xlim(ax(i),[0,200]);
        liveline(i) = animatedline;
        set(ax(i),'YTick',80:5:180);
        ylim(ax(i),[70,170]);
        grid(ax(i), 'on');
        grid(ax(i), 'minor');
        grid on;
        %xlim([0,1000]);
        %ylim([20,120]);
    end
    while(1)
        if(state == 1)
            bytes = fread(s, numbytes, 'uint8');
            
            if(counter == 202)
                counter = 202; 
            else
                counter = counter + 1;
            end
            for i = 1:4
                pressure(i) = (bytes(2*i-1)*256 + bytes(2*i))*conv;
                if size(val_list{i},2)<202
                    val_list{i}=[val_list{i},pressure(i)];
                else
                    val_list{i}(1:200)=val_list{i}(2:201);
                    val_list{i}(201)=pressure(i);
                end
                set(gcf,'Renderer','OpenGL');
                clearpoints(liveline(i));
                addpoints(liveline(i),x(1:counter), val_list{i});                    
                drawnow limitrate 
            end
            t = datestr(now, 'dd-mmm-yyyy HH:MM:SS.FFF');
            %t_char = t(13:24);
            %t_num = datetime(t_char, 'InputFormat', fmt, 'Format', fmt);
            %t_w = timetable(t_num,(1:10).');
            fprintf(fid,'%s,',t(13:24));
            fprintf(fid,'%f,%f,%f,%f',pressure);
            fprintf(fid,'\n');
            state = 0;
        else
           out = fread(s,1,'uint8');
            if(out=='s')
                state=1;
            end
        end
            
    end
    