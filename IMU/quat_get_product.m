
function out = quat_get_product(q1, q2)

%         Quaternion getProduct(Quaternion q) {
%             // Quaternion multiplication is defined by:
%             //     (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
%             //     (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
%             //     (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
%             //     (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
%             return Quaternion(
%                 w*q.w - x*q.x - y*q.y - z*q.z,  // new w
%                 w*q.x + x*q.w + y*q.z - z*q.y,  // new x
%                 w*q.y - x*q.z + y*q.w + z*q.x,  // new y
%                 w*q.z + x*q.y - y*q.x + z*q.w); // new z
%         }

    out(:,1) = q1(:,1).* q2(:,1) - q1(:,2) .* q2(:,2) - q1(:,3) .* q2(:,3) - q1(:,4) .* q2(:,4);
    out(:,2) = q1(:,1).* q2(:,2) + q1(:,2) .* q2(:,1) + q1(:,3) .* q2(:,4) - q1(:,4) .* q2(:,3);
    out(:,3) = q1(:,1).* q2(:,3) - q1(:,2) .* q2(:,4) + q1(:,3) .* q2(:,1) + q1(:,4) .* q2(:,2);
    out(:,4) = q1(:,1).* q2(:,4) + q1(:,2) .* q2(:,3) - q1(:,3) .* q2(:,2) + q1(:,4) .* q2(:,1);
end

