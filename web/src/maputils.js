const radiusAtEquator = 637813700, // in cenitmeter
      radiusAtPole = 635675230;


// more useful stuff https://github.com/chrisveness/geodesy      

/**
 * Return the earth circumference (in centimeters) at given latitude and height above sea
 * @param {*} latitude 
 * @param {*} heightAboveSea height above sealevel (in meters)
 */
export function earthCircumference(latitude, heightAboveSea) {

  latitude *= Math.PI / 180;
  // taken from https://rechneronline.de/earth-radius
  // latitude B, radius R, radius at equator r1, radius at pole r2
  // R = √ [ (r1² * cos(B))² + (r2² * sin(B))² ] / [ (r1 * cos(B))² + (r2 * sin(B))² ] 
  let radius = Math.sqrt(
    (Math.pow(radiusAtEquator * radiusAtEquator * Math.cos(latitude), 2) + Math.pow(radiusAtPole * radiusAtPole * Math.sin(latitude), 2)) /
    (Math.pow(radiusAtEquator * Math.cos(latitude), 2) + Math.pow(radiusAtPole * Math.sin(latitude), 2))
  ) + heightAboveSea * 100;

  return 2 * radius * Math.PI;
}
