function [R_d] = LinearOptimization(P_after, Np, P_origin)

    
    % We have the relation P_after = R_d * P_origin, but we should
    % convert the equation into "P_after = P_origin * R_d" form, because we 
    % want to solve R_d, not P_origin

    % implement your code here
    P1=[];P2=[];

    for i = 1:Np
        P1 = cat(1,P1,matrix_change(P_origin(:,i)'));
        P2 = cat(1,P2,P_after(:,i));
    end

    R_d = inv((P1')*P1)*(P1')*P2;
    R_d = reshape(R_d,3,3);

end
