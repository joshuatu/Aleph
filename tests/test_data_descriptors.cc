#include <aleph/containers/PointCloud.hh>
#include <aleph/containers/DataDescriptors.hh>

#include <aleph/geometry/distances/Euclidean.hh>
#include <aleph/geometry/distances/Manhattan.hh>

#include <aleph/utilities/ContainerOperators.hh>

#include <tests/Base.hh>

#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

#include <cmath>

using namespace aleph::geometry;
using namespace aleph::containers;
using namespace aleph;

std::vector<double> eccentricities0Euclidean = { 6.49846135635198,6.53146231712317,6.71788657242737,6.59014415623816,6.55362495112438,6.134329629226,6.67083203206317,6.44204936336256,6.75573830162181,6.48768063332344,6.33008688723938,6.44592894779333,6.60757141467272,7.08519583356734,6.51690110405245,6.32534584034739,6.47688196588451,6.4668384856899,6.0398675482166,6.43117407632541,6.11391854705311,6.38278935889318,7.05903676148524,6.12617335699864,6.20161269348546,6.32060123722419,6.29523629421486,6.37416661219332,6.44592894779333,6.46993044784872,6.41560597293818,6.22093240599832,6.51920240520265,6.48536814683638,6.48768063332344,6.6745786383861,6.43583716388163,6.48768063332344,6.8425141578224,6.4007812023221,6.5924198895398,6.75943784644848,6.85711309517351,6.25139984323511,6.06712452484701,6.54369925959315,6.37965516309463,6.68056883805563,6.36710295189264,6.51459898996093,4.68828326789242,4.23910367884533,4.8135226186235,3.89743505398102,4.36921045499069,3.91279950930277,4.39317652729776,4.77493455452533,4.35775171390018,4.00874045056549,4.71274866717927,3.7589892258425,3.76563407675255,4.23083916026124,4.04845649599943,4.28368999812078,3.91152144312159,3.62629287289375,4.29651021178817,3.89230009120571,4.37949768809164,3.62353418639869,4.58257569495584,4.19285105864733,4.01248052954778,4.22729227756965,4.65510472492295,4.85077313425396,4.05215991787096,4.15331193145904,4.05215991787096,4.15210789840534,3.71887079635741,4.64112055434892,3.86522961801754,4.10609303352956,4.54862616621766,4.16533311993171,3.66196668472011,3.82491829978106,3.79077828420497,4.14487635521254,3.65239647354994,4.74763098818769,3.64142829120663,3.57910603363466,3.62629287289375,3.91152144312159,4.83942145302514,3.58887168898527,5.81893461039046,4.68508271004899,5.90592922409336,5.21056618804521,5.59821400091136,6.71863081289633,4.02492235949962,6.24019230472908,5.5874860178796,6.28728876384726,4.94873721266345,5.04281667324919,5.44150714416512,4.65403051128804,4.87134478352744,5.18941229813165,5.20480547186924,6.92603782836912,7.08519583356734,4.64865571966778,5.72712842531054,4.48218696620299,6.82422156732913,4.63357313528124,5.56866231693034,5.9455865984779,4.4977772288098,4.53541618817943,5.35723809439155,5.7227615711298,6.15548535860496,6.73052746818554,5.39536838408648,4.69361268108053,5.08428952755446,6.43428317685817,5.44977063737548,5.16430053347014,4.41248229458204,5.40925133451941,5.59553393341511,5.25452186216786,4.68508271004899,5.84551109827019,5.72450871254468,5.23545604508337,4.79478883789474,5.02593274925163,5.20576603392815,4.63141447076376 };
std::vector<double> eccentricities1Euclidean = { 2.89079980416499,2.92938125424666,3.03641983689583,2.9621727303441,2.92929541313731,2.78705043697625,3.00602715597502,2.8356746106249,3.1373108792989,2.88701630933734,2.86529976759286,2.84126021185023,2.98522430007638,3.39123238649493,3.19483936246115,3.1337014835966,3.01260481744661,2.86805628876277,2.7979536274294,2.88954147893703,2.71306352350348,2.84120141277709,3.3362862387173,2.66715124686446,2.72665726890308,2.79976852903545,2.74652913581179,2.82290483495452,2.87035491460546,2.86651939236302,2.83533038097471,2.77334415186149,3.05451006705864,3.11804356416122,2.88701630933734,3.02718807256835,2.95881428992775,2.88701630933734,3.18248503470553,2.81932709558314,2.95185464618982,3.3012992406306,3.17206200994729,2.75134649646765,2.6964284226437,2.93496451471652,2.86202324080246,3.01392571947907,2.86361870791345,2.887876772943,2.25210981624465,1.98512518050107,2.22746828456605,2.06886133885497,2.00615773108091,1.94520613495742,2.02291842913504,2.31223098752768,2.04205581494509,2.06744085792057,2.39397607950724,1.89835739861966,2.10542320720528,1.94672340807722,1.98132245022511,2.064855598057,1.96659337777104,1.9495229263458,2.11266343543384,1.99819636135933,2.04616553157592,1.91727190637452,2.09222422433019,1.97790012609079,1.95547714086482,2.01741835159988,2.16200700729576,2.17088900883612,1.90872975252756,2.03899524868891,2.05313241230174,2.07671946279796,1.92853794674744,2.10725672425492,2.02954017351314,2.00145923145597,2.09349202211143,2.07505815822128,1.92596797654544,2.00175141894978,2.01277077432431,1.92721872317531,1.937498872848,2.30437405125727,1.93860121574396,1.91643906296722,1.90486515310751,1.91168528955325,2.2819233419764,1.90787144992946,2.88569024639774,2.18061648043976,2.83758488737405,2.37657898289289,2.61941662819479,3.56065886078822,2.33380495257112,3.15346857309184,2.64681560466341,3.22648403277086,2.22207743224167,2.26147462958443,2.48846372584116,2.23921550054667,2.37402053213599,2.39187689849027,2.34093891793156,3.82029521693236,3.91546616857478,2.22918241718016,2.71143603019124,2.1752503389485,3.67806596357303,2.07206551693359,2.59780194308357,2.90770716199425,2.01994415413271,2.036083682264,2.45916815198835,2.76464216367847,3.08321021435105,3.68792107059849,2.49383774441842,2.10251707257203,2.40689422435585,3.33293597247376,2.61261436224734,2.3308243191516,2.01122902482044,2.48572105721239,2.62695720495105,2.4561754939334,2.18061648043976,2.79894792696815,2.7528270066953,2.40633037809812,2.17849941999448,2.23753954254663,2.46512342412505,2.1243706113791 };
std::vector<double> eccentricities2Euclidean = { 3.442518457951,3.46298522472543,3.59301173576337,3.49080697451654,3.47977010734905,3.2135753712441,3.54617164465192,3.38654199245582,3.63406108919484,3.42629051113105,3.35434047168739,3.37702038292141,3.52145803136523,3.89861513873837,3.60273969824817,3.48001915703539,3.48338915425767,3.41484992349591,3.18772646254348,3.39956860008247,3.17655998421773,3.35382368846863,3.8717437931764,3.14294978218446,3.19359776219026,3.30304909641582,3.26610879590173,3.35418146994663,3.41072817249728,3.39478030707929,3.35956346370576,3.2549244742902,3.51503437631365,3.53986817080335,3.42629051113105,3.57986964380921,3.44934776443315,3.42629051113105,3.69693566800037,3.36303434416005,3.50736368231183,3.71005840025913,3.69942337849202,3.23530524062259,3.12096139034112,3.4642748158886,3.3623007995518,3.55902608401044,3.36901568216396,3.44546078195646,2.61240629816012,2.34742412017939,2.65535936048839,2.30081145106098,2.41520185491814,2.27678135387071,2.41972450222472,2.47108613096886,2.41933875263469,2.26683332720633,2.5430952269495,2.19784743176894,2.32172349774903,2.35691889267889,2.15805467956676,2.39304547943968,2.28939584461345,2.19590528028875,2.45677295111562,2.22141096302928,2.45139416115266,2.17663348621979,2.53923873106357,2.35697546303167,2.27502380939922,2.35926542240023,2.5774923213594,2.65854596850735,2.28636538345617,2.20723054225576,2.25740263725076,2.26436157301199,2.16471707158234,2.57855256555559,2.31919526272944,2.32244124432317,2.50034664263444,2.39530095534291,2.1743351474263,2.24324170194238,2.29654813433843,2.31430335090282,2.192046228223,2.46341226756708,2.22,2.18107007376349,2.18818037038388,2.23451709921107,2.44613981611845,2.17960241022684,3.39483922054246,2.63762519450104,3.39391612546136,2.91945200337324,3.18475535428809,4.06364368516729,2.55666449369747,3.67483332955387,3.17935003839883,3.721934980625,2.72629174276464,2.80028569970994,3.04985245544764,2.65505806590616,2.80135681411704,2.90837870069678,2.89254213452458,4.26575511095828,4.39011009732862,2.63142546920866,3.26472050871127,2.57047336755963,4.16462883500239,2.55674271942512,3.14871402321646,3.43305500489967,2.48526323220164,2.50436951480141,3.0192935155982,3.27267474705324,3.60558825528743,4.11630902630014,3.05063927726632,2.58879637412189,2.87086514254269,3.83193597719656,3.11461073009132,2.87135275900866,2.45199238715512,3.02718791399983,3.17939197541501,2.94441844852256,2.63762519450104,3.35761820342933,3.28564554793524,2.92677296693816,2.66215451592628,2.77354165884223,2.94833286225736,2.58954307423787 };

void truncatedGaussianTest()
{
  ALEPH_TEST_BEGIN( "Truncated Gaussian density estimation test" );

  auto pc = load<double>( CMAKE_SOURCE_DIR + std::string( "/tests/input/Iris_comma_separated.txt" ) );
  auto g1 = estimateDensityTruncatedGaussian( pc, 0.1 );
  auto g2 = estimateDensityTruncatedGaussian( pc, 0.2 );
  auto g3 = estimateDensityTruncatedGaussian( pc, 1.0 );

  ALEPH_ASSERT_THROW( g1.size() == pc.size() );
  ALEPH_ASSERT_THROW( g2.size() == pc.size() );
  ALEPH_ASSERT_THROW( g3.size() == pc.size() );

  auto pcSorted = load<double>( CMAKE_SOURCE_DIR + std::string( "/tests/input/Iris_comma_separated_sorted.txt" ) );
  auto g1Sorted = estimateDensityTruncatedGaussian( pc, 0.1 );
  auto g2Sorted = estimateDensityTruncatedGaussian( pc, 0.2 );
  auto g3Sorted = estimateDensityTruncatedGaussian( pc, 1.0 );

  std::sort( g1.begin(), g1.end() );
  std::sort( g2.begin(), g2.end() );
  std::sort( g3.begin(), g3.end() );

  std::sort( g1Sorted.begin(), g1Sorted.end() );
  std::sort( g2Sorted.begin(), g2Sorted.end() );
  std::sort( g3Sorted.begin(), g3Sorted.end() );

  ALEPH_ASSERT_THROW( aleph::utilities::allclose( g1.begin(), g1.end(), g1Sorted.begin(), g1Sorted.end() ) );
  ALEPH_ASSERT_THROW( aleph::utilities::allclose( g2.begin(), g2.end(), g2Sorted.begin(), g2Sorted.end() ) );
  ALEPH_ASSERT_THROW( aleph::utilities::allclose( g3.begin(), g3.end(), g3Sorted.begin(), g3Sorted.end() ) );

  ALEPH_TEST_END();
}

template <class D> void eccentricityTest()
{
  ALEPH_TEST_BEGIN( "Eccentricity test" );

  auto pc = load<double>( CMAKE_SOURCE_DIR + std::string( "/tests/input/Iris_comma_separated.txt" ) );
  auto e0 = eccentricities<D>( pc, 0 );
  auto e1 = eccentricities<D>( pc );
  auto e2 = eccentricities<D>( pc, 2 );

  ALEPH_ASSERT_THROW( e0.empty() == false );

  ALEPH_ASSERT_EQUAL( e0.size(), pc.size() );
  ALEPH_ASSERT_EQUAL( e0.size(), e1.size() );
  ALEPH_ASSERT_EQUAL( e0.size(), e2.size() );

  // Only run this test for the Euclidean distance. I have no ground
  // truth values for the other distance measures.
  if( std::is_same<D, aleph::geometry::distances::Euclidean<typename D::ResultType> >::value )
  {
    ALEPH_ASSERT_THROW( aleph::utilities::allclose( e0.begin(), e0.end(), eccentricities0Euclidean.begin(), eccentricities0Euclidean.end() ) );
    ALEPH_ASSERT_THROW( aleph::utilities::allclose( e1.begin(), e1.end(), eccentricities1Euclidean.begin(), eccentricities1Euclidean.end() ) );
    ALEPH_ASSERT_THROW( aleph::utilities::allclose( e2.begin(), e2.end(), eccentricities2Euclidean.begin(), eccentricities2Euclidean.end() ) );
  }

  auto pcSorted = load<double>( CMAKE_SOURCE_DIR + std::string( "/tests/input/Iris_comma_separated_sorted.txt" ) );
  auto e0Sorted = eccentricities<D>( pcSorted, 0 );
  auto e1Sorted = eccentricities<D>( pcSorted, 1 );
  auto e2Sorted = eccentricities<D>( pcSorted, 2 );

  ALEPH_ASSERT_EQUAL( pc.size(),      pcSorted.size()      );
  ALEPH_ASSERT_EQUAL( pc.dimension(), pcSorted.dimension() );

  std::sort( e0Sorted.begin(), e0Sorted.end() );
  std::sort( e1Sorted.begin(), e1Sorted.end() );
  std::sort( e2Sorted.begin(), e2Sorted.end() );

  std::sort( e0.begin(), e0.end() );
  std::sort( e1.begin(), e1.end() );
  std::sort( e2.begin(), e2.end() );

  ALEPH_ASSERT_THROW( aleph::utilities::allclose( e0.begin(), e0.end(), e0Sorted.begin(), e0Sorted.end() ) );
  ALEPH_ASSERT_THROW( aleph::utilities::allclose( e1.begin(), e1.end(), e1Sorted.begin(), e1Sorted.end() ) );
  ALEPH_ASSERT_THROW( aleph::utilities::allclose( e2.begin(), e2.end(), e2Sorted.begin(), e2Sorted.end() ) );

  ALEPH_TEST_END();
}

template <class D> void distanceToMeasureTest()
{
  ALEPH_TEST_BEGIN( "Distance to a measure density estimator test" );

  auto pc = load<double>( CMAKE_SOURCE_DIR + std::string( "/tests/input/Iris_comma_separated.txt" ) );
  auto e1 = estimateDensityDistanceToMeasure<D>( pc,  1 );
  auto e2 = estimateDensityDistanceToMeasure<D>( pc,  5 );
  auto e3 = estimateDensityDistanceToMeasure<D>( pc, 10 );

  ALEPH_ASSERT_THROW( e1.empty() == false );
  ALEPH_ASSERT_THROW( e1.size()  == pc.size() );
  ALEPH_ASSERT_THROW( e1.size()  == e2.size() );

  auto pcSorted = load<double>( CMAKE_SOURCE_DIR + std::string( "/tests/input/Iris_comma_separated_sorted.txt" ) );
  auto e1Sorted = estimateDensityDistanceToMeasure<D>( pcSorted,  1 );
  auto e2Sorted = estimateDensityDistanceToMeasure<D>( pcSorted,  5 );
  auto e3Sorted = estimateDensityDistanceToMeasure<D>( pcSorted, 10 );

  std::sort( e1Sorted.begin(), e1Sorted.end() );
  std::sort( e2Sorted.begin(), e2Sorted.end() );
  std::sort( e3Sorted.begin(), e3Sorted.end() );

  std::sort( e1.begin(), e1.end() );
  std::sort( e2.begin(), e2.end() );
  std::sort( e3.begin(), e3.end() );

  ALEPH_ASSERT_THROW( aleph::utilities::allclose( e1.begin(), e1.end(), e1Sorted.begin(), e1Sorted.end() ) );
  ALEPH_ASSERT_THROW( aleph::utilities::allclose( e2.begin(), e2.end(), e2Sorted.begin(), e2Sorted.end() ) );
  ALEPH_ASSERT_THROW( aleph::utilities::allclose( e3.begin(), e3.end(), e3Sorted.begin(), e3Sorted.end() ) );

  ALEPH_TEST_END();
}

int main()
{
  using T  = double;
  using ED = distances::Euclidean<T>;
  using MD = distances::Manhattan<T>;

  std::cerr << "-- Euclidean distance\n";

  truncatedGaussianTest();

  eccentricityTest<ED>();
  distanceToMeasureTest<ED>();

  std::cerr << "-- Manhattan distance\n";

  eccentricityTest<MD>();
  distanceToMeasureTest<MD>();
}
